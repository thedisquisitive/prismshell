#include "prismshell/parser.hpp"
#include "prismshell/lexer.hpp"
#include <stdexcept>
#include <cctype>
#include <memory>

namespace pb {

/* ------------------- basic stream utilities ------------------- */
Token Parser::pop() { return ts[i++]; }
bool  Parser::match(TokKind k) { if (!eof() && ts[i].k == k) { ++i; return true; } return false; }
// case-insensitive uppercase helper for Id tokens
static inline std::string up(const std::string& s){
  std::string r = s; for(char& c: r) c = (char)std::toupper((unsigned char)c); return r;
}

static inline bool is_cmp(TokKind k) {
  switch (k) {
    case TokKind::Eq: case TokKind::Ne: case TokKind::Lt:
    case TokKind::Le: case TokKind::Gt: case TokKind::Ge: return true;
    default: return false;
  }
}

/* ------------------- expression parsing ------------------- */
/* Factor supports: numbers, strings, variables, calls, (expr),
   and unary + / - (implemented as 0±factor) */
ExprPtr Parser::parseFactor() {
  if (eof()) return nullptr;

  // unary +/- handled here
  if (match(TokKind::Minus)) {
    auto rhs = parseFactor();
    auto zero = std::make_shared<Expr>(); zero->kind = Expr::Num; zero->line = peek().line; zero->val = 0.0;
    auto b = std::make_shared<Expr>(); b->kind = Expr::Bin; b->op = '-'; b->left = zero; b->right = rhs;
    return b;
  }
  if (match(TokKind::Plus)) {
    return parseFactor();
  }

  const Token& t = peek();

  if (t.k == TokKind::Num) {
    auto n = std::make_shared<Expr>();
    n->kind = Expr::Num; n->line = t.line; n->val = std::stod(t.text);
    pop(); return n;
  }
  if (t.k == TokKind::Str) {
    auto s = std::make_shared<Expr>();
    s->kind = Expr::Str; s->line = t.line; s->val = t.text;
    pop(); return s;
  }
  if (t.k == TokKind::Id) {
    std::string name = t.text;
    int savedLine = t.line;
    pop();
    
    // Check for array indexing: name[expr]
    if (match(TokKind::LBracket)) {
      auto idx = parseExpr();
      match(TokKind::RBracket);  // consume closing bracket
      auto arr = std::make_shared<Expr>();
      arr->kind = Expr::ArrIndex;
      arr->line = savedLine;
      arr->arrName = name;
      arr->index = idx;
      return arr;
    }
    
    // Check for function call: name(args)
    if (match(TokKind::LParen)) {
      auto call = std::make_shared<Expr>();
      call->kind = Expr::CallFn; call->name = name; call->line = savedLine;
      if (!match(TokKind::RParen)) {
        while (true) {
          auto e = parseExpr();
          if (!e) break;
          call->args.push_back(e);
          if (match(TokKind::RParen)) break;
          if (!match(TokKind::Comma)) break;
        }
      }
      return call;
    }
    
    // Plain variable
    auto v = std::make_shared<Expr>();
    v->kind = Expr::Var; v->line = savedLine; v->name = name;
    return v;
  }
  if (match(TokKind::LParen)) {
    auto e = parseExpr();
    match(TokKind::RParen);
    return e;
  }
  return nullptr;
}

ExprPtr Parser::parseTerm() {
  auto left = parseFactor();

  // exponentiation (higher precedence than * and /)
  while (!eof() && peek().k == TokKind::Caret) {
    pop(); // '^'
    auto right = parseFactor();
    auto b = std::make_shared<Expr>();
    b->kind = Expr::Bin; b->op = '^'; b->left = left; b->right = right;
    left = b;
  }

  // multiplication / division
  while (!eof() && (peek().k == TokKind::Star || peek().k == TokKind::Slash)) {
    char op = (peek().k == TokKind::Star ? '*' : '/');
    pop();
    auto right = parseFactor();
    auto b = std::make_shared<Expr>();
    b->kind = Expr::Bin; b->op = op; b->left = left; b->right = right;
    left = b;
  }
  return left;
}


ExprPtr Parser::parseExpr() {
  // additive layer
  auto left = parseTerm();
  while (!eof() && (peek().k == TokKind::Plus || peek().k == TokKind::Minus)) {
    char op = (peek().k == TokKind::Plus ? '+' : '-');
    pop();
    auto right = parseTerm();
    auto b = std::make_shared<Expr>();
    b->kind = Expr::Bin; b->op = op; b->left = left; b->right = right;
    left = b;
  }

  // optional single comparison with lower precedence
  if (!eof() && is_cmp(peek().k)) {
    std::string cmp;
    switch (peek().k) {
      case TokKind::Eq: cmp = "=="; break;
      case TokKind::Ne: cmp = "!="; break;
      case TokKind::Lt: cmp = "<";  break;
      case TokKind::Le: cmp = "<="; break;
      case TokKind::Gt: cmp = ">";  break;
      case TokKind::Ge: cmp = ">="; break;
      default: break;
    }
    pop();
    auto right = parseTerm(); // right-assoc with same precedence as +/-
    auto b = std::make_shared<Expr>();
    b->kind = Expr::Bin; b->cmp = cmp; b->left = left; b->right = right;
    left = b;
  }
  return left;
}

/* ------------------- statement parsing ------------------- */

StmtPtr Parser::parseStmt() {
  if (eof()) return nullptr;
  const Token& t = peek();

  // REM should consume the rest of the line
  if (t.k == TokKind::Rem) {
    pop();
    while (!eof() && peek().k != TokKind::End) pop();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Rem; s->line = t.line;
    return s;
  }

  // NEW: DIM name[size] or DIM name[]
  if (t.k == TokKind::Dim) {
    pop();  // consume DIM
    if (peek().k != TokKind::Id) return nullptr;
    std::string name = pop().text;
    if (!match(TokKind::LBracket)) return nullptr;
    
    ExprPtr size = nullptr;
    if (peek().k != TokKind::RBracket) {
      size = parseExpr();  // parse size expression
    }
    
    if (!match(TokKind::RBracket)) return nullptr;
    
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Dim;
    s->line = t.line;
    s->dimName = name;
    s->dimSize = size;  // nullptr means dynamic []
    return s;
  }

  // --- WHILE / WEND (no lexer keyword needed; accept Id "WHILE"/"WEND") ---
  if (!eof() && peek().k == TokKind::Id) {
    std::string u = up(peek().text);
    if (u == "WHILE") {
      pop();
      auto cond = parseExpr();
      auto s = std::make_shared<Stmt>();
      s->kind = Stmt::While; s->line = t.line; s->ifCond = cond;
      return s;
    }
    if (u == "WEND") {
      pop();
      auto s = std::make_shared<Stmt>();
      s->kind = Stmt::Wend; s->line = t.line;
      return s;
    }
  }

  // --- FOR/NEXT (Phase 2) ---
  if (t.k == TokKind::For) {
    auto t_for = pop();
    
    // Expect: variable name
    if (peek().k != TokKind::Id) return nullptr;
    std::string var = pop().text;
    
    // Expect: =
    if (!match(TokKind::Eq)) return nullptr;
    
    // Parse start expression
    auto start = parseExpr();
    if (!start) return nullptr;
    
    // Expect: TO
    if (!match(TokKind::To)) return nullptr;
    
    // Parse end expression
    auto end = parseExpr();
    if (!end) return nullptr;
    
    // Optional: STEP
    ExprPtr step = nullptr;
    if (match(TokKind::Step)) {
      step = parseExpr();
      if (!step) return nullptr;
    }
    
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::For;
    s->line = t_for.line;
    s->forVar = var;
    s->forStart = start;
    s->forEnd = end;
    s->forStep = step;  // nullptr means default 1
    return s;
  }

  // NEXT [var]
  if (t.k == TokKind::Next) {
    auto t_next = pop();
    
    // Optional: variable name
    std::string var;
    if (peek().k == TokKind::Id) {
      var = pop().text;
    }
    
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Next;
    s->line = t_next.line;
    s->forVar = var;  // may be empty
    return s;
  }
  
  // LET or array assignment detection
  if (t.k == TokKind::Let) {
    pop();
    if (peek().k != TokKind::Id) return nullptr;
    std::string name = pop().text;
    
    // Check for array assignment: LET name[index] = expr
    if (peek().k == TokKind::LBracket) {
      pop();  // consume [
      auto idx = parseExpr();
      if (!match(TokKind::RBracket)) return nullptr;
      if (!match(TokKind::Eq)) return nullptr;
      auto val = parseExpr();
      
      auto s = std::make_shared<Stmt>();
      s->kind = Stmt::ArrAssign;
      s->line = t.line;
      s->arrName = name;
      s->arrIndex = idx;
      s->arrValue = val;
      return s;
    }
    
    // Regular LET
    if (!match(TokKind::Eq)) return nullptr;
    auto e = parseExpr();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Let; s->line = t.line; s->letName = name; s->letExpr = e;
    return s;
  }

  if (t.k==TokKind::Print){
    pop();
    bool newline = true;
    ExprPtr e = parseExpr();
    if(!e){
      auto z = std::make_shared<Expr>();
      z->kind = Expr::Str;
      z->line = t.line;
      z->val  = std::string("");
      e = z;
    }

    while(!eof()){
      if(peek().k == TokKind::Comma || peek().k == TokKind::Semi){
        bool isSemi = (peek().k == TokKind::Semi);
        pop();
        if(peek().k == TokKind::End){
          if(isSemi) newline = false;
          break;
        }
        auto rhs = parseExpr();
        if(!rhs){
          auto z = std::make_shared<Expr>();
          z->kind = Expr::Str;
          z->line = t.line;
          z->val  = std::string("");
          rhs = z;
        }
        auto b = std::make_shared<Expr>();
        b->kind  = Expr::Bin;
        b->op    = '+';
        b->left  = e;
        b->right = rhs;
        e = b;
        continue;
      }
      break;
    }

    auto s = std::make_shared<Stmt>();
    s->kind         = Stmt::Print;
    s->line         = t.line;
    s->printExpr    = e;
    s->printNewline = newline;
    return s;
  }

  if (t.k == TokKind::Input) {
    pop();
    if (peek().k != TokKind::Id) return nullptr;
    std::string name = pop().text;
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Input; s->line = t.line; s->inputVar = name;
    return s;
  }

  if (t.k == TokKind::If) {
    auto t_if = pop();
    auto cond = parseExpr();
    if (!match(TokKind::Then)) return nullptr;

    // MULTILINE IF if line ends right after THEN
    if (peek().k == TokKind::End) {
      auto s = std::make_shared<Stmt>();
      s->kind = Stmt::IfThenBlk; s->line = t_if.line; s->ifCond = cond;
      return s;
    }

    // SINGLE-LINE IF ... THEN <line>
    if (peek().k == TokKind::Goto) pop();
    if (peek().k == TokKind::Num) {
      int tgt = std::stoi(pop().text);
      auto s = std::make_shared<Stmt>();
      s->kind = Stmt::If; s->line = t_if.line; s->ifCond = cond; s->thenLine = tgt;
      return s;
    }
    return nullptr;
  }

  // ELSEIF <expr> THEN
  if (!eof() && peek().k == TokKind::Id && up(peek().text) == "ELSEIF") {
    auto t_ei = pop();
    auto cond = parseExpr();
    if (!match(TokKind::Then)) return nullptr;
    if (peek().k != TokKind::End) return nullptr;
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::ElseIfThen; s->line = t_ei.line; s->ifCond = cond;
    return s;
  }

  // ELSE
  if (!eof() && peek().k == TokKind::Else) {
    auto t_else = pop();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::ElseBlk; s->line = t_else.line;
    return s;
  }

  // ENDIF
  if (!eof() && peek().k == TokKind::Id && up(peek().text) == "ENDIF") {
    auto t_endif = pop();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::EndIf; s->line = t_endif.line;
    return s;
  }

  if (t.k == TokKind::Goto) {
    pop();
    if (peek().k != TokKind::Num) return nullptr;
    int tgt = std::stoi(pop().text);
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Goto; s->line = t.line; s->targetLine = tgt;
    return s;
  }

  if (t.k == TokKind::Gosub) {
    pop();
    if (peek().k != TokKind::Num) return nullptr;
    int tgt = std::stoi(pop().text);
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Gosub; s->line = t.line; s->targetLine = tgt;
    return s;
  }

  if (t.k == TokKind::ReturnTok) {
    pop();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Return; s->line = t.line;
    return s;
  }

  if (t.k == TokKind::Call) {
    pop();
    if (peek().k != TokKind::Id) return nullptr;
    std::string qn = pop().text;
    if (!match(TokKind::LParen)) return nullptr;
    std::vector<ExprPtr> args;
    if (!match(TokKind::RParen)) {
      while (true) {
        auto e = parseExpr();
        if (!e) break;
        args.push_back(e);
        if (match(TokKind::RParen)) break;
        if (!match(TokKind::Comma)) break;
      }
    }
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Call; s->line = t.line; s->callName = qn; s->callArgs = std::move(args);
    return s;
  }

  if (t.k == TokKind::EndTok) {
    pop();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::End; s->line = t.line;
    return s;
  }

  return nullptr;
}

/* ------------------- toplevel ------------------- */
ParseOut Parser::parse() {
  ParseOut out;
  while (!eof()) {
    auto s = parseStmt();
    if (!s) {
      out.err = Error{ peek().line, "Parse error" };
      break;
    }
    out.stmts.push_back(s);
    if (peek().k == TokKind::End) break;
  }
  return out;
}

} // namespace pb
