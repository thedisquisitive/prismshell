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
  enum Kind { 
    Num, Str, Var, Bin, CallFn,
    ArrIndex   // array[index] access
  } kind;
  int line{0};

  // literals
  Value val;                 // for Num/Str

  // identifiers / calls
  std::string name;          // for Var or CallFn
  std::vector<ExprPtr> args; // for CallFn

  // for ArrIndex
  std::string arrName;
  ExprPtr index;

  // binary arithmetic
  char op{0};                // + - * / ^

  // comparisons
  std::string cmp;           // "==","!=", "<","<=",">",">="
  ExprPtr left, right;
};

// NEW: Support for READ with array indexing
struct ReadTarget {
  std::string varName;
  ExprPtr arrayIndex;  // nullptr for simple variables, expr for array[index]
};

struct Stmt {
  enum Kind {
    Rem, Let, Print, Input,
    If, Goto, Gosub, Return, Call, End,
    While, Wend,
    IfThenBlk,   // "IF <expr> THEN" (EOL) – block header
    ElseIfThen,  // "ELSEIF <expr> THEN"
    ElseBlk,     // "ELSE"
    EndIf,       // "ENDIF"
    Dim,         // DIM name[size] or DIM name[]
    ArrAssign,   // name[index] = expr
    SubDef,      // SUB definition (parsed separately)
    For,         // FOR var = start TO end [STEP step]
    Next,        // NEXT [var]
    Data,        // DATA value1, value2, ...
    Read,        // READ var1, var2, ... (now supports arrays)
    Restore      // RESTORE
  } kind{};
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

  // DIM
  std::string dimName;
  ExprPtr dimSize;  // nullptr means dynamic []

  // ArrAssign
  std::string arrName;
  ExprPtr arrIndex;
  ExprPtr arrValue;

  // SubDef
  std::string subName;
  std::vector<std::string> subParams;

  // FOR loop
  std::string forVar;
  ExprPtr forStart;
  ExprPtr forEnd;
  ExprPtr forStep;  // nullptr means step 1

  // DATA
  std::vector<ExprPtr> dataValues;

  // READ - updated to support arrays
  std::vector<ReadTarget> readTargets;
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

// Is the first token of a line an Id equal to kw (case-insensitive)?
static bool first_token_is_id_kw(const std::string& src, int line, const char* kw){
  Lexer lx(src, line);
  auto ts = lx.lex();
  if(ts.empty()) return false;
  const auto& t = ts[0];
  if(t.k != TokKind::Id) return false;
  std::string u = t.text;
  for(char& c : u) c = (char)std::toupper((unsigned char)c);
  return u == kw;
}

} // namespace pb