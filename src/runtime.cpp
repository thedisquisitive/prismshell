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

#ifndef _WIN32
  #include <unistd.h>
  #include <glob.h>
#else
  #include <windows.h>
#endif
#include <algorithm>

namespace fs = std::filesystem;

namespace pb {

/* ---------------- internal helpers ---------------- */

static Value num(double d)              { return Value{d}; }
static Value str(std::string s)         { return Value{std::move(s)}; }

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

/* ---------------- Runtime: expression eval ---------------- */

Value Runtime::eval(const ExprPtr& e){
  if(!e) return {};
  switch(e->kind){
    case Expr::Num: return e->val;
    case Expr::Str: return e->val;

    case Expr::Var: {
      if(e->name == "_") return lastCall;
      auto it = vars.find(e->name);
      return it == vars.end() ? Value{} : it->second;
    }

    case Expr::CallFn: {
      std::vector<Value> args; args.reserve(e->args.size());
      for(const auto& a : e->args) args.push_back(eval(a));
      return call_dispatch(*this, e->name, args);
    }

    case Expr::Bin: {
      if(!e->cmp.empty()){
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
      if(e->op=='/') return num(rn == 0.0 ? 0.0 : ln / rn);
      return {};
    }
  }
  return {};
}

/* ---------------- Runtime: stmt exec ---------------- */

Result Runtime::exec(const StmtPtr& s, int* pc, std::vector<int>& gosubStack){
  Result r;
  switch(s->kind){
    case Stmt::Rem: break;

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

    case Stmt::Goto: {
      *pc = s->targetLine;
    } break;

    case Stmt::Gosub: {
      gosubStack.push_back(*pc);
      *pc = s->targetLine;
    } break;

    case Stmt::Return: {
      if(gosubStack.empty()) r.err = Error{s->line, "RETURN without GOSUB"};
      else { *pc = gosubStack.back(); gosubStack.pop_back(); }
    } break;

    case Stmt::Call: {
      std::vector<Value> args; args.reserve(s->callArgs.size());
      for(const auto& a : s->callArgs) args.push_back(eval(a));
      lastCall = call_dispatch(*this, s->callName, args);
      vars["_"] = lastCall;
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
    int lineNo = lines[i];
    std::string src = program[lineNo];

    Lexer lx(src, lineNo);
    Parser p(lx.lex());
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
  std::string line;
  while(std::getline(f, line)){
    std::string t = trim(line);
    if(t.empty()) continue;
    auto sp = t.find(' ');
    if(sp == std::string::npos) continue;
    int n = std::stoi(t.substr(0, sp));
    std::string src = trim(t.substr(sp + 1));
    program[n] = src;
  }
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

Value call_dispatch(Runtime& rt, const std::string& qname, const std::vector<Value>& args){
  std::string up = qname;
  for(char& c : up) c = (char)std::toupper((unsigned char)c);

  auto wantN = [&](size_t n){ return args.size() == n; };
  auto asS   = [&](size_t i){ return std::holds_alternative<std::string>(args[i]) ? std::get<std::string>(args[i]) : to_string(args[i]); };
  auto asD   = [&](size_t i){ return std::holds_alternative<Number>(args[i])     ? std::get<Number>(args[i])     : std::stod(to_string(args[i])); };

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

  // ------- UI.* stubs for CLI builds
  if(starts_with(up, "UI.")) return Value{};

  // ------- PrismFS verbs (stubbed)
  if(up=="MOUNT.ADD" || up=="SNAPSHOT" || up=="BRANCH" || up=="CHECKOUT") return Value{};

  // Unknown -> empty
  return Value{};
}

} // namespace pb
