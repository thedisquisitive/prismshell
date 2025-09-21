#pragma once
#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "prismshell/utils.hpp"   // Value, Error, helpers
#include "prismshell/lexer.hpp"   // Token, TokKind

namespace pb {

struct Stmt;
struct Expr;

using StmtPtr = std::shared_ptr<Stmt>;
using ExprPtr = std::shared_ptr<Expr>;

struct Expr {
  enum Kind { Num, Str, Var, Bin, CallFn } kind;
  int line{0};

  // literals
  Value val;                 // for Num/Str

  // identifiers / calls
  std::string name;          // for Var or CallFn
  std::vector<ExprPtr> args; // for CallFn

  // binary arithmetic
  char op{0};                // + - * /

  // comparisons
  std::string cmp;           // "==","!=", "<","<=",">",">="
  ExprPtr left, right;
};

struct Stmt {
  enum Kind { Let, Print, Input, If, Goto, Gosub, Return, Call, End, Rem } kind;
  int line{0};

  // LET
  std::string letName; ExprPtr letExpr;

  // PRINT
  ExprPtr printExpr; bool printNewline{true};

  // INPUT
  std::string inputVar;

  // IF ... THEN <line>
  ExprPtr ifCond; int thenLine{-1};

  // CALL
  std::string callName; std::vector<ExprPtr> callArgs;

  // GOTO/GOSUB targets
  int targetLine{-1};
};

struct ParseOut {
  std::vector<StmtPtr> stmts;
  std::optional<Error> err;
};

struct Parser {
  std::vector<Token> ts;
  int i{0};

  explicit Parser(std::vector<Token> toks) : ts(std::move(toks)) {}
  ParseOut parse();

private:
  bool eof() const { return i >= (int)ts.size() || ts[i].k == TokKind::End; }
  const Token& peek() const { return ts[i]; }
  Token pop();
  bool match(TokKind k);

  ExprPtr parseExpr();
  ExprPtr parseTerm();
  ExprPtr parseFactor();

  StmtPtr parseStmt();
};

} // namespace pb
