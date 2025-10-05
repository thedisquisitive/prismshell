#include "prismshell/runtime.hpp"
#include "prismshell/parser.hpp"
#include "prismshell/lexer.hpp"
#include "prismshell/utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <system_error>
#include <cstdlib>
#include <cctype>
#include <limits>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <random>
#include <iomanip>
#include <cmath>

#ifndef _WIN32
  #include <unistd.h>
  #include <glob.h>
#else
  #include <windows.h>
#endif
#include <algorithm>
#include <atomic>
#include <csignal>

namespace fs = std::filesystem;

namespace pb {

  // Take the first token as uppercase string (or "")
  static inline std::string first_kw(const std::string& src, int line){
      Lexer lx(src, line);
      auto ts = lx.lex();
      if(ts.empty()) return "";
      
      // Handle keywords by their TokKind
      switch(ts[0].k) {
        case TokKind::Print:     return "PRINT";
        case TokKind::Let:       return "LET";
        case TokKind::Input:     return "INPUT";
        case TokKind::If:        return "IF";
        case TokKind::Then:      return "THEN";
        case TokKind::Else:      return "ELSE";
        case TokKind::EndTok:    return "END";
        case TokKind::Goto:      return "GOTO";
        case TokKind::Gosub:     return "GOSUB";
        case TokKind::ReturnTok: return "RETURN";
        case TokKind::Call:      return "CALL";
        case TokKind::Dim:       return "DIM";
        case TokKind::Sub:       return "SUB";
        case TokKind::Rem:       return "REM";
        case TokKind::For:       return "FOR";
        case TokKind::Next:      return "NEXT";
        case TokKind::To:        return "TO";
        case TokKind::Step:      return "STEP";
        case TokKind::Data:      return "DATA";
        case TokKind::Read:      return "READ";
        case TokKind::Restore:   return "RESTORE";
        default:
          // For identifiers (like WHILE, WEND, ENDIF, ELSEIF), use text
          if(ts[0].k == TokKind::Id) {
            std::string u = ts[0].text;
            for(char& c: u) c = (char)std::toupper((unsigned char)c);
            return u;
          }
          return "";
      }
    }

  // Return the next line number after 'line' (or max if end)
  static inline int next_line_after(const std::map<int,std::string>& program, int line){
    auto it = program.upper_bound(line);
    return (it == program.end()) ? std::numeric_limits<int>::max() : it->first;
  }

  // ArrayData implementation
  size_t ArrayData::linearIndex(const std::vector<int>& indices) const {
    if (indices.size() != dimensions.size()) {
      return 0;  // Dimension mismatch
    }
    
    size_t idx = 0;
    size_t multiplier = 1;
    
    // Row-major order (C-style): last index varies fastest
    for (int i = (int)dimensions.size() - 1; i >= 0; --i) {
      if (indices[i] < 0) return 0;  // Negative index
      idx += (size_t)indices[i] * multiplier;
      multiplier *= dimensions[i];
    }
    
    return idx;
  }

  void ArrayData::ensureCapacity(const std::vector<int>& indices) {
    if (!isDynamic) return;
    
    bool needsResize = false;
    std::vector<size_t> newDims = dimensions;
    
    // Expand any dimension that's too small
    for (size_t i = 0; i < indices.size(); ++i) {
      if (indices[i] < 0) continue;  // Skip negative indices
      
      size_t needed = (size_t)indices[i] + 1;
      if (needed > newDims[i]) {
        newDims[i] = needed;
        needsResize = true;
      }
    }
    
    if (needsResize) {
      dimensions = newDims;
      
      // Calculate total size
      size_t totalSize = 1;
      for (auto d : dimensions) {
        totalSize *= d;
      }
      
      data.resize(totalSize);
    }
  }


/* ---------------- SIGINT handling for program run ---------------- */
namespace {
  inline std::atomic_bool g_rt_sigint{false};

  void rt_on_sigint(int){ g_rt_sigint.store(true, std::memory_order_relaxed); }

  struct RtSigintScope {
    using Handler = void (*)(int);
    Handler prev{};
    RtSigintScope() : prev(std::signal(SIGINT, rt_on_sigint)) {}
    ~RtSigintScope(){ std::signal(SIGINT, prev); g_rt_sigint.store(false, std::memory_order_relaxed); }
  };

  inline bool rt_interrupted(){ return g_rt_sigint.load(std::memory_order_relaxed); }
}


/* ---------------- internal helpers ---------------- */

static Value num(double d)              { return Value{d}; }
static Value str(std::string s)         { return Value{std::move(s)}; }

// Helper: Find the line after the matching NEXT for a given FOR
static int find_matching_next(const std::map<int,std::string>& program, 
                               int forLine, 
                               const std::string& /*varName*/) {
  int depth = 0;
  for (auto it = program.upper_bound(forLine); it != program.end(); ++it) {
    int ln = it->first;
    std::string kw = first_kw(it->second, ln);
    
    if (kw == "FOR") {
      ++depth;
    } else if (kw == "NEXT") {
      if (depth == 0) {
        // Found matching NEXT - return line after it
        auto it2 = program.upper_bound(ln);
        return (it2 == program.end()) ? std::numeric_limits<int>::max() : it2->first;
      }
      --depth;
    }
  }
  // No matching NEXT found
  return std::numeric_limits<int>::max();
}

// Helper: Convert Value to double
static double asDouble(const Value& v) {
  if (std::holds_alternative<Number>(v)) {
    return std::get<Number>(v);
  }
  std::string s = to_string(v);
  try {
    return std::stod(s);
  } catch (...) {
    return 0.0;
  }
}

/* ---------------- User SUB execution ---------------- */

// Helper: Extract SUB definitions from program and move them to rt.subs
void extract_subs(Runtime& rt) {
  std::map<int, std::string> newProgram;  // Program without SUB blocks
  
  auto it = rt.program.begin();
  while (it != rt.program.end()) {
    int lineNum = it->first;
    const std::string& line = it->second;
    
    // Check if line starts with SUB keyword
    Lexer lx(line, lineNum);
    auto tokens = lx.lex();
    
    if (!tokens.empty() && tokens[0].k == TokKind::Sub) {
      // Found SUB definition - extract it
      SubDefinition subDef;
      subDef.entryLine = lineNum;
      
      // Parse: SUB name(param1, param2, ...)
      if (tokens.size() >= 2 && tokens[1].k == TokKind::Id) {
        std::string subName = tokens[1].text;
        
        // Extract parameters from (param1, param2, ...)
        size_t i = 2;
        if (i < tokens.size() && tokens[i].k == TokKind::LParen) {
          i++;
          while (i < tokens.size() && tokens[i].k != TokKind::RParen) {
            if (tokens[i].k == TokKind::Id) {
              std::string param = tokens[i].text;
              // Strip [] suffix if present (e.g., args[])
              size_t bracket = param.find('[');
              if (bracket != std::string::npos) {
                param = param.substr(0, bracket);
              }
              subDef.params.push_back(param);
            }
            i++;
            // Skip comma
            if (i < tokens.size() && tokens[i].k == TokKind::Comma) i++;
          }
        }
        
        // Collect SUB body until we find END SUB
        ++it;  // Move past the SUB declaration line
        int depth = 0;  // Track nesting depth for error detection
        
        while (it != rt.program.end()) {
          int bodyLineNum = it->first;
          const std::string& bodyLine = it->second;
          
          // Parse this line to check for END SUB or nested SUB
          Lexer bodyLx(bodyLine, bodyLineNum);
          auto bodyTokens = bodyLx.lex();
          
          // Check for nested SUB (error)
          if (!bodyTokens.empty() && bodyTokens[0].k == TokKind::Sub) {
            std::cerr << "Error at line " << bodyLineNum 
                      << ": Nested SUB not supported\n";
            depth++;
          }
          
          // Check for END SUB (two tokens: END SUB)
          bool isEndSub = false;
          if (bodyTokens.size() >= 2) {
            if (bodyTokens[0].k == TokKind::EndTok && 
                bodyTokens[1].k == TokKind::Sub) {
              isEndSub = true;
            }
          }
          
          // Also check for single-word ENDSUB identifier
          if (!isEndSub && !bodyTokens.empty()) {
            if (bodyTokens[0].k == TokKind::Id) {
              std::string word = bodyTokens[0].text;
              for (char& c : word) c = (char)std::toupper((unsigned char)c);
              if (word == "ENDSUB") isEndSub = true;
            }
          }
          
          if (isEndSub) {
            if (depth > 0) {
              depth--;  // Closing a nested SUB
            } else {
              
              break;  // Done with this SUB
            }
          }
          
          // Add this line to the SUB body
          subDef.body[bodyLineNum] = bodyLine;
          ++it;
        }
        
        // Warn if we never found END SUB
        if (it == rt.program.end() && !subDef.body.empty()) {
          std::cerr << "Warning: SUB " << subName 
                    << " at line " << lineNum << " missing END SUB\n";
        }
        
        // Check for duplicate SUB names
        if (rt.subs.find(subName) != rt.subs.end()) {
          std::cerr << "Warning: SUB " << subName 
                    << " at line " << lineNum << " redefines earlier SUB\n";
        }
        
        // Register the SUB
        rt.subs[subName] = subDef;
      } else {
        // Malformed SUB line - skip it
        std::cerr << "Error at line " << lineNum << ": Malformed SUB declaration\n";
        ++it;
      }
      
      continue;  // Don't add SUB definition lines to main program
    }
    
    // Not a SUB line - keep in main program
    newProgram[lineNum] = line;
    ++it;
  }
  
  // Replace program with version that has SUBs extracted
  rt.program = newProgram;
}

Value call_user_sub(Runtime& rt, const SubDefinition& sub, const std::vector<Value>& args){
  // Create a child runtime that inherits parent's context
  Runtime childRt;
  childRt.vars = rt.vars;        // inherit variables
  childRt.arrays = rt.arrays;    // inherit arrays
  childRt.subs = rt.subs;        // inherit SUB definitions
  childRt.program = sub.body;    // use SUB's body as program
  
  // Bind parameters
  for(size_t i = 0; i < sub.params.size(); ++i){
    if(i < args.size()) {
      childRt.vars[sub.params[i]] = args[i];
    } else {
      childRt.vars[sub.params[i]] = Value{};  // default to empty
    }
  }
  
  // Run the SUB from its entry line
  auto res = childRt.run_program(sub.entryLine);
  
  if(res.err) {
    std::cerr << "Error in SUB at line " << res.err->line << ": " << res.err->msg << "\n";
    return Value{};
  }
  
  // Return value: check _ variable or lastCall
  auto it = childRt.vars.find("_");
  if(it != childRt.vars.end()) {
    return it->second;
  }
  return childRt.lastCall;
}

/* ---------------- Mod registry (in-memory) ---------------- */

struct ModEntry {
  std::string name;
  std::map<int,std::string> program; // the mod's BASIC program listing
  int entry{0};                      // entry line to start from
};

static std::unordered_map<std::string, ModEntry> g_mods;

static void mod_register_from_rt(Runtime& rt, const std::string& name, int entry) {
  ModEntry m;
  m.name   = name;
  m.program= rt.program;  // capture the mod's program as authored
  m.entry  = entry;
  g_mods[name] = std::move(m);
}

bool mod_has(const std::string& name) {
  return g_mods.find(name) != g_mods.end();
}

// Run a mod and capture a resulting string (PROMPT or lastCall string).
int mod_run_capture(const std::string& name, const std::vector<std::string>& args, Runtime& parent, std::string* out) {
  auto it = g_mods.find(name);
  if (it == g_mods.end()) return 127; // not found
  const ModEntry& m = it->second;

  Runtime child;
  child.program = m.program;     // run the mod's program
  child.vars    = parent.vars;   // inherit variables/env

  // Populate arg variables for the mod
  std::ostringstream all;
  for (size_t i=0;i<args.size();++i) {
    if (i) all << " ";
    all << args[i];
    child.vars["PB_ARG"+std::to_string(i+1)] = args[i];
  }
  child.vars["PB_CMD"]  = name;
  child.vars["PB_ARGS"] = all.str();
  child.vars["PB_ARGC"] = (double)args.size();
  child.vars["PB_ARGV"] = all.str();  // space-joined for now

  auto res = child.run_program(m.entry);
  if (res.err) {
    std::cerr << "Mod '"<<name<<"' error at " << res.err->line << ": " << res.err->msg << "\n";
    return 1;
  }

  if (out) {
    // Prefer PROMPT var if set, else child's lastCall, else empty
    auto itp = child.vars.find("PROMPT");
    if (itp != child.vars.end()) *out = to_string(itp->second);
    else                         *out = to_string(child.lastCall);
  }
  return 0;
}

// Backwards-compatible wrapper used by command mods (no capture needed).
int mod_run(const std::string& name, const std::vector<std::string>& args, Runtime& parent) {
  std::string ignored;
  return mod_run_capture(name, args, parent, &ignored);
}

void Runtime::buildDataPool() {
  dataPool.clear();
  dataPointer = 0;
 
  // Iterate through program in line number order
  for (const auto& [lineNum, src] : program) {
    
    // Parse this line to check if it's a DATA statement
    Lexer lx(src, lineNum);
    Parser p(lx.lex());
    auto po = p.parse();
    
    if (po.err) {
      continue;
    }
    
    if (!po.stmts.empty()) {
      for (const auto& stmt : po.stmts) {
        
        if (stmt->kind == Stmt::Data) {
         
          // Evaluate each expression in the DATA statement and add to pool
          for (const auto& expr : stmt->dataValues) {
            Value v = eval(expr);
            dataPool.push_back(v);
           }
        }
      }
    }
  }
  
}

/* ---------------- Runtime: expression eval ---------------- */

Value Runtime::eval(const ExprPtr& e){
  if(!e) return {};
  switch(e->kind){
    case Expr::Num: return e->val;
    case Expr::Str: return e->val;

    case Expr::Var: {
      // Check vars first (including _), then fall back to lastCall only if _ not in vars
      auto it = vars.find(e->name);
      if(it != vars.end()) return it->second;
      if(e->name == "_") return lastCall;
      return Value{};
    }

    // Array indexing with multidimensional support
    case Expr::ArrIndex: {
      auto it = arrays.find(e->arrName);
      if (it == arrays.end()) return Value{};
      
      std::vector<int> indices;
      indices.reserve(e->indices.size());
      
      for (const auto& idxExpr : e->indices) {
        Value idxVal = eval(idxExpr);
        int idx = 0;
        if (std::holds_alternative<Number>(idxVal)) {
          idx = (int)std::get<Number>(idxVal);
        } else {
          std::string s = to_string(idxVal);
          try { idx = std::stoi(s); } catch(...) { idx = 0; }
        }
        indices.push_back(idx);
      }
      
      // Validate dimension count
      if (indices.size() != it->second.dimensions.size()) {
        return Value{};  // Dimension mismatch
      }
      
      // Auto-expand if dynamic
      if (it->second.isDynamic) {
        it->second.ensureCapacity(indices);
      }
      
      size_t linearIdx = it->second.linearIndex(indices);
      if (linearIdx >= it->second.data.size()) return Value{};
      
      return it->second.data[linearIdx];
    }

    case Expr::CallFn: {
      std::vector<Value> args; args.reserve(e->args.size());
      for(const auto& a : e->args) args.push_back(eval(a));
      
      // Check if it's a user SUB first
      auto subIt = subs.find(e->name);
      if(subIt != subs.end()) {
        return call_user_sub(*this, subIt->second, args);
      }
      
      // Otherwise dispatch to builtins
      return call_dispatch(*this, e->name, args);
    }

    case Expr::Bin: {
      if(!e->cmp.empty()){
        // Handle logical operators FIRST (with short-circuit evaluation)
        if(e->cmp == "&&") {
          Value L = eval(e->left);
          if (!truthy(L)) return num(0.0);  // Short-circuit: false AND anything = false
          Value R = eval(e->right);
          return num(truthy(R) ? 1.0 : 0.0);
        }
        
        if(e->cmp == "||") {
          Value L = eval(e->left);
          if (truthy(L)) return num(1.0);   // Short-circuit: true OR anything = true
          Value R = eval(e->right);
          return num(truthy(R) ? 1.0 : 0.0);
        }
        
        // Other comparison operators
        Value L = eval(e->left), R = eval(e->right);
        std::string ls = to_string(L), rs = to_string(R);
        double ln = std::holds_alternative<Number>(L) ? std::get<Number>(L) : std::atof(ls.c_str());
        double rn = std::holds_alternative<Number>(R) ? std::get<Number>(R) : std::atof(rs.c_str());
        bool ok = false;
        if(e->cmp=="==") ok = (ls==rs) || (ln==rn);
        else if(e->cmp=="!=") ok = !((ls==rs) || (ln==rn));
        else if(e->cmp=="<")  ok = ln <  rn;
        else if(e->cmp=="<=") ok = ln <= rn;
        else if(e->cmp==">")  ok = ln >  rn;
        else if(e->cmp==">=") ok = ln >= rn;
        return num(ok ? 1.0 : 0.0);
      }

      Value L = eval(e->left), R = eval(e->right);
      if(e->op == '+'){
        if(std::holds_alternative<std::string>(L) || std::holds_alternative<std::string>(R))
          return str(to_string(L) + to_string(R));
        double ln = std::holds_alternative<Number>(L) ? std::get<Number>(L) : std::atof(to_string(L).c_str());
        double rn = std::holds_alternative<Number>(R) ? std::get<Number>(R) : std::atof(to_string(R).c_str());
        return num(ln + rn);
      }
      double ln = std::holds_alternative<Number>(L) ? std::get<Number>(L) : std::atof(to_string(L).c_str());
      double rn = std::holds_alternative<Number>(R) ? std::get<Number>(R) : std::atof(to_string(R).c_str());
      if(e->op=='-') return num(ln - rn);
      if(e->op=='*') return num(ln * rn);
      if (e->op == '^') return num(std::pow(ln, rn));
      if(e->op=='/') return num(rn == 0.0 ? 0.0 : ln / rn);
      return {};
    }
  }
  return {};
}


/* ---------------- Runtime: stmt exec ---------------- */

Result Runtime::exec(const StmtPtr& s, int* pc, std::vector<int>& gosubStack){
  Result r;
  if (rt_interrupted()) { return Result{ Error{ s ? s->line : 0, "Interrupted (Ctrl-C)" } }; }

  switch(s->kind){
    case Stmt::Rem: break;

    // DIM statement with Multidimensional Array Support
    case Stmt::Dim: {
      ArrayData arr;
      arr.dimensions.reserve(s->dimSizes.size());
      
      size_t totalSize = 1;
      bool hasStaticDims = false;
      
      for (const auto& sizeExpr : s->dimSizes) {
        if (!sizeExpr) {
          // Dynamic dimension (nullptr or empty)
          arr.dimensions.push_back(0);
          arr.isDynamic = true;
        } else {
          int dim = (int)asDouble(eval(sizeExpr));
          if (dim < 0) dim = 0;
          arr.dimensions.push_back(dim);
          totalSize *= dim;
          hasStaticDims = true;
        }
      }
      
      // Only pre-allocate if fully static
      if (hasStaticDims && !arr.isDynamic) {
        arr.data.resize(totalSize);
      }
      
      arrays[s->dimName] = arr;
    } break;

    // Array assignment with multidimensional support
    case Stmt::ArrAssign: {
      auto it = arrays.find(s->arrName);
      if (it == arrays.end()) {
        // Auto-create dynamic array
        ArrayData arr;
        arr.isDynamic = true;
        arr.dimensions.resize(s->arrIndices.size(), 0);
        arrays[s->arrName] = arr;
        it = arrays.find(s->arrName);
      }
      
      std::vector<int> indices;
      indices.reserve(s->arrIndices.size());
      
      for (const auto& idxExpr : s->arrIndices) {
        Value idxVal = eval(idxExpr);
        int idx = 0;
        if (std::holds_alternative<Number>(idxVal)) {
          idx = (int)std::get<Number>(idxVal);
        } else {
          std::string str = to_string(idxVal);
          try { idx = std::stoi(str); } catch(...) { idx = 0; }
        }
        indices.push_back(idx);
      }
      
      // Validate dimension count
      if (indices.size() != it->second.dimensions.size()) {
        break;  // Dimension mismatch - skip assignment
      }
      
      // Auto-expand if needed
      if (it->second.isDynamic) {
        it->second.ensureCapacity(indices);
      }
      
      size_t linearIdx = it->second.linearIndex(indices);
      if (linearIdx < it->second.data.size()) {
        it->second.data[linearIdx] = eval(s->arrValue);
      }
    } break;

    // NEW: SubDef - should not appear during execution
    case Stmt::SubDef: {
      // No-op during execution; SUBs are registered before running
    } break;

    case Stmt::Let: {
      vars[s->letName] = eval(s->letExpr);
    } break;

    case Stmt::Print: {
      auto v = eval(s->printExpr);
      std::cout << to_string(v);
      if(s->printNewline) std::cout << "\n";
    } break;

    case Stmt::Input: {
      std::cout << s->inputVar << "? ";
      std::string line; std::getline(std::cin, line);
      vars[s->inputVar] = line;
    } break;

    case Stmt::If: {
      if(truthy(eval(s->ifCond))) *pc = s->thenLine;
    } break;

    // IF ... THEN   (block header)
    case Stmt::IfThenBlk: {
  // If condition is FALSE: jump to first satisfied ELSEIF, or ELSE, or after ENDIF
  if(!truthy(eval(s->ifCond))){
    int cur = s->line;
    int depth = 0;
    int jump = std::numeric_limits<int>::max();

    for(auto it = program.upper_bound(cur); it != program.end(); ++it){
      int ln = it->first;
      std::string kw = first_kw(it->second, ln);

      if(kw == "IF"){ ++depth; continue; }
      if(kw == "ENDIF"){
        if(depth == 0){ jump = next_line_after(program, ln); break; }
        --depth; continue;
      }

      if(depth > 0) continue;

      if(kw == "ELSEIF"){
        // Evaluate this ELSEIF's condition: lex/parse just this line
        Lexer lx(it->second, ln);
        Parser p(lx.lex());
        auto po = p.parse();
        if(!po.err && !po.stmts.empty()){
          auto he = po.stmts.front(); // ElseIfThen
          if(truthy(eval(he->ifCond))){
            jump = next_line_after(program, ln); // start executing body lines
            break;
          } else {
            continue; // check next branch
          }
        }
      }
      if(kw == "ELSE"){
        jump = next_line_after(program, ln);
        break;
      }
    }

    *pc = jump;
  }
} break;

    // ELSEIF ... THEN   (if we *fall through* here, a previous branch already ran → skip to ENDIF)
    case Stmt::ElseIfThen: {
      // Skip to after matching ENDIF at same nesting depth
      int cur = s->line, depth = 0;
      int jump = std::numeric_limits<int>::max();
      for(auto it = program.upper_bound(cur); it != program.end(); ++it){
        int ln = it->first;
        std::string kw = first_kw(it->second, ln);
        if(kw == "IF"){ ++depth; continue; }
        if(kw == "ENDIF"){
          if(depth == 0){ jump = next_line_after(program, ln); break; }
          --depth;
        }
      }
      *pc = jump;
    } break;

    // ELSE   (fallthrough means a prior branch ran → skip to ENDIF)
    case Stmt::ElseBlk: {
      int cur = s->line, depth = 0;
      int jump = std::numeric_limits<int>::max();
      for(auto it = program.upper_bound(cur); it != program.end(); ++it){
        int ln = it->first;
        std::string kw = first_kw(it->second, ln);
        if(kw == "IF"){ ++depth; continue; }
        if(kw == "ENDIF"){
          if(depth == 0){ jump = next_line_after(program, ln); break; }
          --depth;
        }
      }
      *pc = jump;
    } break;

    // ENDIF — no-op
    case Stmt::EndIf: {
      // nothing
    } break;

    case Stmt::Goto: {
      *pc = s->targetLine;
    } break;

    case Stmt::Gosub: {
      // return address = first line strictly greater than current
      auto it = program.upper_bound(*pc);
      int ret = (it == program.end()) ? std::numeric_limits<int>::max() : it->first;
      gosubStack.push_back(ret);
      *pc = s->targetLine;
    } break;

    case Stmt::Return: {
      if(gosubStack.empty()) r.err = Error{s->line, "RETURN without GOSUB"};
      else { *pc = gosubStack.back(); gosubStack.pop_back(); }
    } break;

    case Stmt::Call: {
      std::vector<Value> args; args.reserve(s->callArgs.size());
      for(const auto& a : s->callArgs) args.push_back(eval(a));
      
      // Check user SUBs first
      auto subIt = subs.find(s->callName);
      if(subIt != subs.end()) {
        lastCall = call_user_sub(*this, subIt->second, args);
      } else {
        lastCall = call_dispatch(*this, s->callName, args);
      }
      vars["_"] = lastCall;
    } break;

    case Stmt::While: {
      // If condition is false, skip to after matching WEND (handle nesting)
      if(!truthy(eval(s->ifCond))){
        int cur = s->line;
        int depth = 0;
        int target = std::numeric_limits<int>::max();
        for(auto it = program.upper_bound(cur); it != program.end(); ++it){
          int ln = it->first;
          const std::string& src = it->second;
          if(first_token_is_id_kw(src, ln, "WHILE")) { ++depth; continue; }
          if(first_token_is_id_kw(src, ln, "WEND")){
            if(depth == 0){
              auto it2 = program.upper_bound(ln);
              target = (it2 == program.end()) ? std::numeric_limits<int>::max() : it2->first;
              break;
            } else {
              --depth;
            }
          }
        }
        *pc = target;
      }
    } break;

    case Stmt::Wend: {
      // Jump back to matching WHILE to re-check the condition
      int cur = s->line;
      int depth = 0;
      int target = program.begin()->first;
      auto it = program.lower_bound(cur);
      while(it != program.begin()){
        --it;
        int ln = it->first;
        const std::string& src = it->second;
        if(first_token_is_id_kw(src, ln, "WEND")) { ++depth; continue; }
        if(first_token_is_id_kw(src, ln, "WHILE")){
          if(depth == 0){ target = ln; break; }
          else { --depth; }
        }
      }
      *pc = target;
    } break;

    case Stmt::For: {
      // Evaluate start, end, step expressions
      double start = asDouble(eval(s->forStart));
      double end = asDouble(eval(s->forEnd));
      double step = s->forStep ? asDouble(eval(s->forStep)) : 1.0;
      
      // Initialize loop variable
      vars[s->forVar] = num(start);
      
      // Determine if we should enter the loop based on step direction
      bool enter = (step > 0) ? (start <= end) : (start >= end);
      
      if (enter) {
        // Push loop state onto stack for NEXT to use
        ForLoopState state;
        state.var = s->forVar;
        state.end = end;
        state.step = step;
        state.loopStartLine = next_line_after(program, s->line);
        forStack.push_back(state);
      } else {
        // Condition false from start - skip to after matching NEXT
        *pc = find_matching_next(program, s->line, s->forVar);
      }
    } break;

    case Stmt::Next: {
      if (forStack.empty()) {
        r.err = Error{s->line, "NEXT without FOR"};
        break;
      }
      
      ForLoopState& state = forStack.back();
      
      // Verify variable name if provided (optional check)
      if (!s->forVar.empty() && s->forVar != state.var) {
        r.err = Error{s->line, "NEXT variable mismatch: expected " + state.var + ", got " + s->forVar};
        break;
      }
      
      // Increment loop variable
      double current = asDouble(vars[state.var]);
      current += state.step;
      vars[state.var] = num(current);
      
      // Check if we should continue looping
      bool cont = (state.step > 0) ? (current <= state.end) : (current >= state.end);
      
      if (cont) {
        // Continue loop - jump back to first line after FOR
        *pc = state.loopStartLine;
      } else {
        // Exit loop - pop state and continue
        forStack.pop_back();
      }
    } break;

    case Stmt::Data: {
      // No-op
    } break;

    case Stmt::Read: {
      for (const auto& target : s->readTargets) {
        if (dataPointer >= dataPool.size()) {
          r.err = Error{s->line, "Out of DATA"};
          break;
        }
        
        Value data = dataPool[dataPointer++];
        
        if (target.arrayIndex) {
          // Reading into array element
          auto it = arrays.find(target.varName);
          if (it == arrays.end()) {
            // Auto-create 1D dynamic array if it doesn't exist
            ArrayData arr;
            arr.isDynamic = true;
            arr.dimensions = {0};
            arrays[target.varName] = arr;
            it = arrays.find(target.varName);
          }
          
          // Evaluate the index expression
          Value idxVal = eval(target.arrayIndex);
          int idx = 0;
          if (std::holds_alternative<Number>(idxVal)) {
            idx = (int)std::get<Number>(idxVal);
          } else {
            std::string str = to_string(idxVal);
            try { idx = std::stoi(str); } catch(...) { idx = 0; }
          }
          
          // Ensure capacity for 1D array access
          std::vector<int> indices = {idx};
          if (it->second.isDynamic) {
            it->second.ensureCapacity(indices);
          }
          
          if (idx >= 0) {
            size_t linearIdx = it->second.linearIndex(indices);
            if (linearIdx < it->second.data.size()) {
              it->second.data[linearIdx] = data;
            }
          }
        } else {
          // Reading into simple variable
          vars[target.varName] = data;
        }
      }
    } break;

    case Stmt::Restore: {
      dataPointer = 0;  // Reset to beginning of DATA pool
    } break;

    case Stmt::End: {
      *pc = std::numeric_limits<int>::max();
    } break;
  }
  return r;
}

/* ---------------- Runtime: single-line / program exec ---------------- */

Result Runtime::run_line_direct(const std::string& line, int lineNo){
  Result r;
  Lexer lx(line, lineNo);
  Parser p(lx.lex());
  auto out = p.parse();
  if(out.err){ r.err = out.err; return r; }

  int pc = lineNo;
  std::vector<int> gs;
  for(const auto& st : out.stmts){
          if (rt_interrupted()) { r.err = Error{ lineNo, "Interrupted (Ctrl-C)" }; break; }
    if (rt_interrupted()) { r.err = Error{ lineNo, "Interrupted (Ctrl-C)" }; break; }
    auto rr = exec(st, &pc, gs);
    if(rr.err){ r.err = rr.err; break; }
  }
  return r;
}

Result Runtime::run_program(){
  return run_program(-1);
}

Result Runtime::run_program(int startLine){
  Result r;
  RtSigintScope _rt_sig_scope;  // enable Ctrl-C -> interrupt during program run
  buildDataPool();
  if(program.empty()) return r;

  std::vector<int> lines; lines.reserve(program.size());
  for(auto& kv : program) lines.push_back(kv.first);
  std::sort(lines.begin(), lines.end());

  std::map<int,int> idx; for(size_t k=0;k<lines.size();++k) idx[lines[k]] = (int)k;

  int i = 0;
  if(startLine >= 0) {
    auto it = idx.lower_bound(startLine);
    i = (it==idx.end()) ? (int)lines.size() : it->second;
  }

  std::vector<int> gosubStack;

  while(i >= 0 && i < (int)lines.size()){
    if (rt_interrupted()) { r.err = Error{ lines[i], "Interrupted (Ctrl-C)" }; break; }
    int lineNo = lines[i];
    std::string src = program[lineNo];

    Lexer lx(src, lineNo);
    auto tokens = lx.lex();

    Parser p(tokens);
    auto out = p.parse();  
    
    if(out.err){ r.err = out.err; break; }

    int pc = lineNo;
    for(const auto& st : out.stmts){
      auto rr = exec(st, &pc, gosubStack);
      if(rr.err){ r.err = rr.err; i = (int)lines.size(); break; }

      if(pc != lineNo){
        auto it = idx.lower_bound(pc);
        if(it == idx.end() || it->first != pc) i = it == idx.end() ? (int)lines.size() : it->second;
        else i = it->second;
        goto next_iter;
      }
    }
    ++i;
    next_iter: ;
  }

  return r;
}

/* ---------------- Runtime: editor helpers ---------------- */

void Runtime::list(){
  for(const auto& kv : program){
    std::cout << kv.first << " " << kv.second << "\n";
  }
}

bool Runtime::save(const std::string& path){
  std::ofstream f(path);
  if(!f) return false;
  for(const auto& kv : program)
    f << kv.first << " " << kv.second << "\n";
  return true;
}

bool Runtime::load(const std::string& path){
  program.clear();

  std::ifstream f(path);
  if(!f) return false;

  // Read entire file so we can handle BOM/shebang and detect format
  std::string content((std::istreambuf_iterator<char>(f)),
                      std::istreambuf_iterator<char>());

  // Strip UTF-8 BOM if present
  if(content.size() >= 3 &&
     (unsigned char)content[0] == 0xEF &&
     (unsigned char)content[1] == 0xBB &&
     (unsigned char)content[2] == 0xBF){
    content.erase(0, 3);
  }

  // Strip shebang if present
  if(!content.empty() && content.rfind("#!", 0) == 0){
    auto nl = content.find('\n');
    content = (nl == std::string::npos) ? std::string() : content.substr(nl+1);
  }

  

  // Helpers
  auto is_all_digits = [](const std::string& s)->bool{
    if(s.empty()) return false;
    for(unsigned char c : s) if(!std::isdigit(c)) return false;
    return true;
  };

  auto is_comment_line = [](const std::string& t)->bool{
    if(t.empty()) return false;
    // Leading apostrophe comment
    if(t[0] == '\'') return true;
    // Case-insensitive "REM" at start
    if(t.size() >= 3){
      unsigned char c0 = (unsigned char)t[0];
      unsigned char c1 = (unsigned char)t[1];
      unsigned char c2 = (unsigned char)t[2];
      if(std::toupper(c0) == 'R' && std::toupper(c1) == 'E' && std::toupper(c2) == 'M')
        return true;
    }
    return false;
  };

  // Decide if numbered (first non-empty, non-comment line begins with digit)
  bool numbered = true;
  {
    std::istringstream iss(content);
    std::string line;
    while(std::getline(iss, line)){
      std::string t = trim(line);
      if(t.empty()) continue;
      if(is_comment_line(t)) continue;
      numbered = std::isdigit((unsigned char)t[0]) != 0;
      break;
    }
  }

  if(numbered){
    // Classic: "10 PRINT ...", tolerant parsing
    std::istringstream iss(content);
    std::string line;
    while(std::getline(iss, line)){
      std::string t = trim(line);
      if(t.empty()) continue;
      auto sp = t.find(' ');
      if(sp == std::string::npos) continue;     // ignore malformed lines
      std::string num = t.substr(0, sp);
      if(!is_all_digits(num)) continue;         // skip non-numeric leaders
      int n = 0;
      try { n = std::stoi(num); } catch(...) { continue; } // guard stoi
      std::string src = trim(t.substr(sp + 1));
      program[n] = src;
    }
  } else {
    // Free-form BASIC: auto-number at 10,20,30,...; skip blank & comment lines
    std::istringstream iss(content);
    std::string line; int n = 10;
    while(std::getline(iss, line)){
      std::string t = trim(line);
      if(t.empty()) continue;
      if(is_comment_line(t)) continue;
      program[n] = t;
      n += 10;
    }
  }
  extract_subs(*this);
  return true;
}

/* ---------------- Runtime: shell passthrough ---------------- */

int Runtime::sh_exec(const std::string& line){
#ifndef _WIN32
  std::string cmd = std::string("/bin/sh -lc \"") + line + "\"";
  return std::system(cmd.c_str());
#else
  return std::system(line.c_str());
#endif
}

/* ---------------- Builtin CALLs ---------------- */

#ifndef _WIN32
static std::vector<std::string> glob_items(const std::string& pat){
  glob_t g{}; std::vector<std::string> out;
  if(glob(pat.c_str(), 0, nullptr, &g) == 0){
    for(size_t i=0; i<g.gl_pathc; ++i) out.emplace_back(g.gl_pathv[i]);
  }
  globfree(&g);
  return out;
}
#endif


static void rng_autoseed(Runtime& rt){
  if (rt.rng_seeded) return;
  uint64_t s = (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
  s ^= (uint64_t)(uintptr_t)&rt;               // per-instance spice
  try {
    std::random_device rd;
    s ^= ((uint64_t)rd() << 32) ^ rd();
  } catch (...) {}                             // okay if rd() not available
  rt.rng.seed(s);
  rt.rng_seeded = true;
}

Value call_dispatch(Runtime& rt, const std::string& qname, const std::vector<Value>& args){
std::string up = qname;
  for(char& c : up) c = (char)std::toupper((unsigned char)c);

  auto wantN = [&](size_t n){ return args.size() == n; };
  auto asS   = [&](size_t i){ return std::holds_alternative<std::string>(args[i]) ? std::get<std::string>(args[i]) : to_string(args[i]); };
  auto asD   = [&](size_t i){ return std::holds_alternative<Number>(args[i])     ? std::get<Number>(args[i])     : std::stod(to_string(args[i])); };

  // NEW: LEN function - works on strings and arrays
  if(up=="LEN" && wantN(1)){
    const Value& v = args[0];
    
    // If it's a string value, return its length
    if(std::holds_alternative<std::string>(v)) {
      return num((double)std::get<std::string>(v).size());
    }
    
    // If it's passed as a variable name (string), check if it's an array
    std::string name = asS(0);
    auto it = rt.arrays.find(name);
    if(it != rt.arrays.end()) {
      // Return size of first dimension (or 0 for empty arrays)
      if (it->second.dimensions.empty()) {
        return num(0.0);
      }
      return num((double)it->second.dimensions[0]);
    }
    
    // For numbers or unknown, return 0
    return num(0.0);
  }

  // ARGV(index) - Get command-line argument by index (0-based)
  if (up == "ARGV" && wantN(1)) {
    int idx = (int)asD(0);
    
    // Check if ARGV array exists
    auto it = rt.arrays.find("ARGV");
    if (it == rt.arrays.end()) {
      return str("");  // No args
    }
    
    if (idx < 0 || idx >= (int)it->second.data.size()) {
      return str("");  // Out of bounds
    }
    
    return it->second.data[idx];
  }

  // ARGC() - Get argument count
  if (up == "ARGC" && wantN(0)) {
    auto it = rt.arrays.find("ARGV");
    if (it == rt.arrays.end()) {
      return num(0.0);
    }
    return num((double)it->second.data.size());
  }

  // ARR.DIMS - Get dimension count
  if (up == "ARR.DIMS" && wantN(1)) {
    std::string name = asS(0);
    auto it = rt.arrays.find(name);
    if (it != rt.arrays.end()) {
      return num((double)it->second.dimensions.size());
    }
    return num(0.0);
  }

  // ------- String manipulation functions -------

  // LEFT(str, n) - leftmost n characters
  if (up == "LEFT" && wantN(2)) {
    std::string s = asS(0);
    int n = (int)asD(1);
    if (n < 0) n = 0;
    if (n > (int)s.size()) n = (int)s.size();
    return str(s.substr(0, n));
  }

  // RIGHT(str, n) - rightmost n characters
  if (up == "RIGHT" && wantN(2)) {
    std::string s = asS(0);
    int n = (int)asD(1);
    if (n < 0) n = 0;
    if (n > (int)s.size()) n = (int)s.size();
    return str(s.substr(s.size() - n, n));
  }

  // MID(str, start, len) - substring from start (1-based) for len chars
  if (up == "MID" && (wantN(2) || wantN(3))) {
    std::string s = asS(0);
    int start = (int)asD(1) - 1;  // Convert to 0-based
    int len = wantN(3) ? (int)asD(2) : (int)s.size();
    
    if (start < 0) start = 0;
    if (start >= (int)s.size()) return str("");
    if (len < 0) len = 0;
    if (start + len > (int)s.size()) len = (int)s.size() - start;
    
    return str(s.substr(start, len));
  }

  // INSTR(haystack, needle) - find position of substring (1-based, 0 if not found)
  if (up == "INSTR" && wantN(2)) {
    std::string haystack = asS(0);
    std::string needle = asS(1);
    size_t pos = haystack.find(needle);
    if (pos == std::string::npos) return num(0.0);
    return num((double)(pos + 1));  // 1-based indexing
  }

  // CHR(code) - ASCII code to character
  if (up == "CHR" && wantN(1)) {
    int code = (int)asD(0);
    if (code < 0 || code > 255) return str("");
    char c = (char)code;
    return str(std::string(1, c));
  }

  // ASC(str) - first character to ASCII code
  if (up == "ASC" && wantN(1)) {
    std::string s = asS(0);
    if (s.empty()) return num(0.0);
    return num((double)(unsigned char)s[0]);
  }

  // UCASE(str) - convert to uppercase
  if (up == "UCASE" && wantN(1)) {
    std::string s = asS(0);
    for (char& c : s) {
      c = (char)std::toupper((unsigned char)c);
    }
    return str(s);
  }

  // LCASE(str) - convert to lowercase
  if (up == "LCASE" && wantN(1)) {
    std::string s = asS(0);
    for (char& c : s) {
      c = (char)std::tolower((unsigned char)c);
    }
    return str(s);
  }

  // TRIM(str) - remove leading and trailing whitespace
  if (up == "TRIM" && wantN(1)) {
    std::string s = asS(0);
    size_t start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start])) {
      ++start;
    }
    if (start >= s.size()) return str("");
    
    size_t end = s.size();
    while (end > start && std::isspace((unsigned char)s[end - 1])) {
      --end;
    }
    
    return str(s.substr(start, end - start));
  }

  // LTRIM(str) - remove leading whitespace
  if (up == "LTRIM" && wantN(1)) {
    std::string s = asS(0);
    size_t start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start])) {
      ++start;
    }
    return str(s.substr(start));
  }

  // RTRIM(str) - remove trailing whitespace
  if (up == "RTRIM" && wantN(1)) {
    std::string s = asS(0);
    size_t end = s.size();
    while (end > 0 && std::isspace((unsigned char)s[end - 1])) {
      --end;
    }
    return str(s.substr(0, end));
  }

  // STR(number) - number to string
  if (up == "STR" && wantN(1)) {
    double n = asD(0);
    std::ostringstream oss;
    oss << std::setprecision(15) << n;
    return str(oss.str());
  }

  // VAL(str) - string to number
  if (up == "VAL" && wantN(1)) {
    std::string s = asS(0);
    try {
      return num(std::stod(s));
    } catch (...) {
      return num(0.0);
    }
  }

  // ARR.SIZE - Get size of specific dimension
  if (up == "ARR.SIZE" && wantN(2)) {
    std::string name = asS(0);
    int dim = (int)asD(1);
    auto it = rt.arrays.find(name);
    if (it != rt.arrays.end() && dim >= 0 && dim < (int)it->second.dimensions.size()) {
      return num((double)it->second.dimensions[dim]);
    }
    return num(0.0);
  }
  // ------- Env.*
  if(up=="ENV.CWD" && wantN(0)) return str(fs::current_path().string());

  if(up=="ENV.ARGS" && wantN(0)){
    auto it = rt.vars.find("PB_ARGV");
    if(it != rt.vars.end()) return it->second;
    return str("[]");
  }

  if(up=="ENV.GET" && wantN(1)){
    const char* v = std::getenv(asS(0).c_str());
    return str(v ? std::string(v) : std::string(""));
  }

  if(up=="ENV.SET" && wantN(2)){
#ifndef _WIN32
    setenv(asS(0).c_str(), asS(1).c_str(), 1);
#else
    _putenv_s(asS(0).c_str(), asS(1).c_str());
#endif
    return str(asS(1));
  }

  if(up=="ENV.EXIT" && wantN(1)){
    std::exit((int)asD(0));
  }

  // ------- TTY.*
  if(up=="TTY.READLINE" && wantN(1)){
    std::cout << asS(0) << std::flush;
    std::string line; std::getline(std::cin, line);
    return str(line);
  }
  if(up=="TTY.WRITE" && wantN(1))     { std::cout << asS(0); return Value{}; }
  if(up=="TTY.WRITELINE" && wantN(1)) { std::cout << asS(0) << "\n"; return Value{}; }

  // ------- FS.* (use error_code to avoid throwing)
  std::error_code ec;

  // ===== New filesystem verbs =====
  if (up=="FS.CD" && wantN(1)) { fs::current_path(asS(0), ec); return num(ec ? 0.0 : 1.0); }
  if (up=="FS.MKDIR" && (wantN(1) || wantN(2))) { bool parents = wantN(2) ? (truthy(args[1])) : true; bool ok = parents ? fs::create_directories(asS(0), ec) : fs::create_directory(asS(0), ec); return num((ok && !ec) ? 1.0 : 0.0); }
  if (up=="FS.RMDIR" && wantN(1)) { bool ok = fs::remove(asS(0), ec); return num((ok && !ec) ? 1.0 : 0.0); }
  if (up=="FS.REMOVEALL" && wantN(1)) { fs::remove_all(asS(0), ec); return num(ec ? 0.0 : 1.0); }
  if (up=="FS.COPY" && (wantN(2) || wantN(3))) { fs::copy_options opts = fs::copy_options::none; bool recursive = wantN(3) ? truthy(args[2]) : false; if (recursive) opts |= fs::copy_options::recursive | fs::copy_options::directories_only; opts |= fs::copy_options::overwrite_existing; fs::copy(asS(0), asS(1), opts, ec); return num(ec ? 0.0 : 1.0); }
  if (up=="FS.MOVE" && wantN(2)) { fs::rename(asS(0), asS(1), ec); if (ec) { ec.clear(); fs::copy(asS(0), asS(1), fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec); if (!ec) { fs::remove_all(asS(0)); } } return num(ec ? 0.0 : 1.0); }
  if (up=="FS.TOUCH" && wantN(1)) { auto p = fs::path(asS(0)); if (!fs::exists(p, ec)) { std::ofstream f(asS(0)); if (!f) return num(0.0); } auto now = fs::file_time_type::clock::now(); fs::last_write_time(p, now, ec); return num(ec ? 0.0 : 1.0); }
  if (up=="FS.SIZE" && wantN(1)) { auto sz = fs::file_size(asS(0), ec); if (ec) return num(0.0); return num((Number)sz); }
  if (up=="FS.ISDIR" && wantN(1)) { bool ok = fs::is_directory(asS(0), ec); return num((ok && !ec) ? 1.0 : 0.0); }
  if (up=="FS.MTIME" && wantN(1)) { auto tp = fs::last_write_time(asS(0), ec); if (ec) return num(0.0); auto s = std::chrono::time_point_cast<std::chrono::seconds>(tp).time_since_epoch().count(); return num((Number)s); }
  // ===== End new filesystem verbs =====

  // ===== New path helpers (pure string transforms) =====
  if (up=="PATH.JOIN" && wantN(2)) { return str( (fs::path(asS(0)) / asS(1)).string() ); }
  if (up=="PATH.DIRNAME" && wantN(1)) { return str( fs::path(asS(0)).parent_path().string() ); }
  if (up=="PATH.BASENAME" && wantN(1)) { return str( fs::path(asS(0)).filename().string() ); }
  if (up=="PATH.EXT" && wantN(1)) { return str( fs::path(asS(0)).extension().string() ); }
  // ===== End path helpers =====


  if(up=="FS.READ" && wantN(1)){
    std::ifstream f(asS(0));
    if(!f) return str("");
    std::stringstream ss; ss << f.rdbuf();
    return str(ss.str());
  }

  if(up=="FS.WRITE" && wantN(2)){
    std::ofstream f(asS(0));
    if(f) f << asS(1);
    return Value{};
  }

  if(up=="FS.APPEND" && wantN(2)){
    std::ofstream f(asS(0), std::ios::app);
    if(f) f << asS(1);
    return Value{};
  }

  if(up=="FS.DELETE" && wantN(1)){
    fs::remove(asS(0), ec);
    return Value{};
  }

  if(up=="FS.LIST" && wantN(1)){
    std::string out;
    fs::directory_iterator endIt;
    fs::directory_iterator it(asS(0), ec);
    while(!ec && it != endIt){
      out += it->path().filename().string();
      out += "\n";
      it.increment(ec);
    }
    return str(out);
  }

  if(up=="FS.EXISTS" && wantN(1)){
    bool ok = fs::exists(asS(0), ec);
    return num((ok && !ec) ? 1.0 : 0.0);
  }

#ifndef _WIN32
  if(up=="FS.GLOB" && wantN(1)){
    std::string out;
    for(const auto& p : glob_items(asS(0))) { out += p; out += "\n"; }
    return str(out);
  }
#else
  if(up=="FS.GLOB" && wantN(1)){
    return str("");
  }
#endif

  // ------- Mod.*
  if(up=="MOD.REGISTER" && wantN(2)) {
    std::string name = asS(0);
    int entry = (int)asD(1);
    mod_register_from_rt(rt, name, entry);
    return Value{};
  }
  if(up=="MOD.LIST" && wantN(0)) {
    std::string out;
    for (auto& kv : g_mods) { out += kv.first; out += "\n"; }
    return str(out);
  }

  // ------- Prompt.* (template control from BASIC/mods)
  if(up=="PROMPT.SETTEMPLATE" && wantN(1)){
    rt.vars["PB_PROMPT_TMPL"] = asS(0);
    return Value{};
  }
  if(up=="PROMPT.GET" && wantN(0)){
    // Interpreter assembles final prompt; here just return stored template (if any)
    auto itp = rt.vars.find("PB_PROMPT_TMPL");
    return itp==rt.vars.end()? str("") : itp->second;
  }

  // --- RNG.* ---------------------------------------------------------------
    if (up=="RNG.SEED" && (wantN(0) || wantN(1))) {
    uint64_t seed;
    if (wantN(0)) {
        // entropy-based seed
        uint64_t s = (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count();
        try { std::random_device rd; s ^= ((uint64_t)rd() << 32) ^ rd(); } catch (...) {}
        seed = s;
    } else {
        seed = (uint64_t)asD(0);
    }
    rt.rng.seed(seed);
    rt.rng_seeded = true;
    return Value{}; // no result
    }

    if (up=="RNG.INT" && (wantN(2) || wantN(1) || wantN(0))) {
    rng_autoseed(rt);
    long long lo = 0, hi = 0x7fffffff; // defaults
    if (wantN(1)) { hi = (long long)asD(0); }
    else if (wantN(2)) { lo = (long long)asD(0); hi = (long long)asD(1); }
    if (hi < lo) std::swap(lo, hi);
    std::uniform_int_distribution<long long> dist(lo, hi);
    return num((Number)dist(rt.rng));
    }

    if (up=="RNG.FLOAT" && wantN(0)) {
    rng_autoseed(rt);
    std::uniform_real_distribution<double> dist(0.0, 1.0); // [0,1)
    return num((Number)dist(rt.rng));
    }

    // Optional friendly alias (classic BASIC vibe)
    if (up=="RAND" && wantN(0)) {
    rng_autoseed(rt);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return num((Number)dist(rt.rng));
    }

  // ------- UI.* stubs for CLI builds
  if(starts_with(up, "UI.")) return Value{};

  // ------- PrismFS verbs (stubbed)
  if(up=="MOUNT.ADD" || up=="SNAPSHOT" || up=="BRANCH" || up=="CHECKOUT") return Value{};

  // ------- TIME.*
  if (up=="TIME.NOW" && wantN(0)) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm{};
  #ifdef _WIN32
    localtime_s(&tm, &t);
  #else
    localtime_r(&t, &tm);
  #endif
    std::ostringstream os;
    os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return str(os.str());
  }

  if(up=="ARR.SET" && wantN(3)){
    std::string arrName = asS(0);
    int idx = (int)asD(1);
    Value val = args[2];
    
    auto it = rt.arrays.find(arrName);
    if(it == rt.arrays.end()) {
      // Create array if needed
      // Create 1D dynamic array
      ArrayData arr;
      arr.isDynamic = true;
      arr.dimensions = {0};
      rt.arrays[arrName] = arr;
      it = rt.arrays.find(arrName);
      }

      // Ensure capacity for 1D array
      std::vector<int> indices = {idx};
      if (it->second.isDynamic) {
        it->second.ensureCapacity(indices);
      }

      if (idx >= 0) {
        size_t linearIdx = it->second.linearIndex(indices);
        if (linearIdx < it->second.data.size()) {
          it->second.data[linearIdx] = val;
        }
      }
    
    return Value{};
  }

  // MAP.SET(mapname, key, value) - Set a key-value pair
  if (up == "MAP.SET" && wantN(3)) {
    std::string mapname = asS(0);
    std::string key = asS(1);
    Value val = args[2];
    
    // Create map if it doesn't exist
    if (rt.maps.find(mapname) == rt.maps.end()) {
      rt.maps[mapname] = std::map<std::string, Value>();
    }
    
    rt.maps[mapname][key] = val;
    return val;
  }

  // MAP.GET(mapname, key [, default]) - Get value for key
  if (up == "MAP.GET" && (wantN(2) || wantN(3))) {
    std::string mapname = asS(0);
    std::string key = asS(1);
    Value default_val = wantN(3) ? args[2] : Value{};
    
    auto map_it = rt.maps.find(mapname);
    if (map_it == rt.maps.end()) {
      return default_val;
    }
    
    auto key_it = map_it->second.find(key);
    if (key_it == map_it->second.end()) {
      return default_val;
    }
    
    return key_it->second;
  }

  // MAP.HAS(mapname, key) - Check if key exists
  if (up == "MAP.HAS" && wantN(2)) {
    std::string mapname = asS(0);
    std::string key = asS(1);
    
    auto map_it = rt.maps.find(mapname);
    if (map_it == rt.maps.end()) {
      return num(0.0);
    }
    
    auto key_it = map_it->second.find(key);
    return num(key_it != map_it->second.end() ? 1.0 : 0.0);
  }

  // MAP.DELETE(mapname, key) - Remove a key
  if (up == "MAP.DELETE" && wantN(2)) {
    std::string mapname = asS(0);
    std::string key = asS(1);
    
    auto map_it = rt.maps.find(mapname);
    if (map_it == rt.maps.end()) {
      return num(0.0);
    }
    
    size_t erased = map_it->second.erase(key);
    return num(erased > 0 ? 1.0 : 0.0);
  }

  // MAP.KEYS(mapname) - Get all keys as newline-separated string
  if (up == "MAP.KEYS" && wantN(1)) {
    std::string mapname = asS(0);
    
    auto map_it = rt.maps.find(mapname);
    if (map_it == rt.maps.end()) {
      return str("");
    }
    
    std::string result;
    for (const auto& kv : map_it->second) {
      if (!result.empty()) result += "\n";
      result += kv.first;
    }
    return str(result);
  }

  // MAP.SIZE(mapname) - Get number of entries
  if (up == "MAP.SIZE" && wantN(1)) {
    std::string mapname = asS(0);
    
    auto map_it = rt.maps.find(mapname);
    if (map_it == rt.maps.end()) {
      return num(0.0);
    }
    
    return num((double)map_it->second.size());
  }

  // MAP.CLEAR(mapname) - Remove all entries
  if (up == "MAP.CLEAR" && wantN(1)) {
    std::string mapname = asS(0);
    
    auto map_it = rt.maps.find(mapname);
    if (map_it != rt.maps.end()) {
      map_it->second.clear();
    }
    
    return Value{};
  }

  // Unknown -> empty
  return Value{};
}  // ← This closes call_dispatch function

} // namespace pb
