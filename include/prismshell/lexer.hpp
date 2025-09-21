#pragma once
#include <string>
#include <vector>


namespace pb {


enum class TokKind {
End, Id, Num, Str,
Plus, Minus, Star, Slash,
LParen, RParen, Comma, Semi,
Eq, Ne, Lt, Le, Gt, Ge,
Let, Print, Input, If, Then, Else, EndTok, Rem,
Goto, Gosub, ReturnTok,
Call,
};


struct Token { TokKind k; std::string text; int line; };


struct Lexer {
std::string src; int pos{0}; int line{0};
explicit Lexer(std::string s, int line0=0): src(std::move(s)), line(line0) {}
std::vector<Token> lex();
};


} // namespace pb