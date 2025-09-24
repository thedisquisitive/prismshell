#include "prismshell/parser.hpp"
#include "prismshell/lexer.hpp"
#include <stdexcept>
#include <cctype>
#include <memory>

namespace pb {

/* ------------------- basic stream utilities ------------------- */
Token Parser::pop() { return ts[i++]; }
bool  Parser::match(TokKind k) { if (!eof() && ts[i].k == k) { ++i; return true; } return false; }

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
    std::string name = t.text; pop();
    if (match(TokKind::LParen)) {
      auto call = std::make_shared<Expr>();
      call->kind = Expr::CallFn; call->name = name; call->line = t.line;
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
    auto v = std::make_shared<Expr>();
    v->kind = Expr::Var; v->line = t.line; v->name = name;
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
    // eat 'REM'
    pop();
    // consume to End token so the parser doesn't see leftovers
    while (!eof() && peek().k != TokKind::End) pop();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Rem; s->line = t.line;
    return s;
  }

  if (t.k == TokKind::Let) {
    pop();
    if (peek().k != TokKind::Id) return nullptr;
    std::string name = pop().text;
    if (!match(TokKind::Eq)) return nullptr;
    auto e = parseExpr();
    auto s = std::make_shared<Stmt>();
    s->kind = Stmt::Let; s->line = t.line; s->letName = name; s->letExpr = e;
    return s;
  }

  if (t.k==TokKind::Print){
  pop();

  bool newline = true;

  // allow bare PRINT -> prints a blank line
  ExprPtr e = parseExpr();
  if(!e){
    auto z = std::make_shared<Expr>();
    z->kind = Expr::Str;
    z->line = t.line;
    z->val  = std::string("");
    e = z;
  }

  // Accept multiple items separated by ';' or ','
  // - We concatenate items with '+' internally.
  // - A trailing ';' (with no following expr) suppresses newline.
  while(!eof()){
    if(peek().k == TokKind::Comma || peek().k == TokKind::Semi){
      bool isSemi = (peek().k == TokKind::Semi);
      pop();

      // Trailing ';' => no newline; nothing more to print
      if(peek().k == TokKind::End){
        if(isSemi) newline = false;
        break;
      }

      // Parse next item and fold into a '+' chain
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

      // If a semicolon was used between items, also suppress newline (classic vibe).
      //if(isSemi) newline = false;
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
    pop();
    auto cond = parseExpr();
    if (!match(TokKind::Then)) return nullptr; // THEN GOTO <line> or THEN <line>
    if (peek().k == TokKind::Goto) pop();
    if (peek().k == TokKind::Num) {
      int tgt = std::stoi(pop().text);
      auto s = std::make_shared<Stmt>();
      s->kind = Stmt::If; s->line = t.line; s->ifCond = cond; s->thenLine = tgt;
      return s;
    }
    return nullptr;
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
