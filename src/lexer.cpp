#include "prismshell/lexer.hpp"
#include "prismshell/utils.hpp"
#include <cctype>

namespace pb {

static bool isid0(char c){ return std::isalpha((unsigned char)c) || c=='_' || c=='.'; }
static bool isid(char c){ return std::isalnum((unsigned char)c) || c=='_' || c=='.'; }

std::vector<Token> Lexer::lex(){
  std::vector<Token> out;
  auto push=[&](TokKind k, std::string t=""){ out.push_back({k,std::move(t),line}); };

  // helper: are we at the start of a (logical) line?
  auto start_of_line = [&](){
    return out.empty() || out.back().line != line;
  };

  // skip leading spaces/tabs only (not newlines)
  auto skipws=[&]{
    while(pos<(int)src.size()){
      char c=src[pos];
      if(c=='\t' || c==' ') { ++pos; }
      else break;
    }
  };

  skipws();
  while(pos<(int)src.size()){
    char c = src[pos];

    // whitespace
    if(c=='\t' || c==' '){ ++pos; continue; }

    // newline / CR: advance line counter
    if(c=='\n'){ ++line; ++pos; skipws(); continue; }
    if(c=='\r'){ ++pos; continue; } // tolerate CR in CRLF

    // single-quote comment: ignore rest of line
    if(c=='\''){
      // If the comment begins the line, emit a Rem token so "REM lines" still parse as a comment stmt.
      if (start_of_line()) push(TokKind::Rem);
      while(pos<(int)src.size() && src[pos] != '\n' && src[pos] != '\r') ++pos;
      // do not emit further tokens from this line
      continue;
    }

    // string
    if(c=='"'){
      std::string s; ++pos;
      while(pos<(int)src.size() && src[pos]!='"'){ s.push_back(src[pos++]); }
      if(pos<(int)src.size() && src[pos]=='"') ++pos;
      push(TokKind::Str, s);
      continue;
    }

    // number
    if(std::isdigit((unsigned char)c)){
      int start=pos;
      while(pos<(int)src.size() && (std::isdigit((unsigned char)src[pos]) || src[pos]=='.')) ++pos;
      push(TokKind::Num, src.substr(start,pos-start));
      continue;
    }

    // identifier / keyword
    if(isid0(c)){
      int start=pos;
      while(pos<(int)src.size() && isid(src[pos])) ++pos;
      std::string id=src.substr(start,pos-start);
      std::string up=id; for(char& ch:up) ch=(char)std::toupper((unsigned char)ch);

      if(up=="LET")        push(TokKind::Let);
      else if(up=="PRINT") push(TokKind::Print);
      else if(up=="INPUT") push(TokKind::Input);
      else if(up=="IF")    push(TokKind::If);
      else if(up=="THEN")  push(TokKind::Then);
      else if(up=="ELSE")  push(TokKind::Else);
      else if(up=="END")   push(TokKind::EndTok);
      else if(up=="REM") {
        // comment keyword: eat rest of line; only emit Rem if at start-of-line
        if (start_of_line()) push(TokKind::Rem);
        while(pos<(int)src.size() && src[pos] != '\n' && src[pos] != '\r') ++pos;
      }
      else if(up=="GOTO")   push(TokKind::Goto);
      else if(up=="GOSUB")  push(TokKind::Gosub);
      else if(up=="RETURN") push(TokKind::ReturnTok);
      else if(up=="CALL")   push(TokKind::Call);
      else                  push(TokKind::Id, id);
      continue;
    }

    // single-char tokens and operators
    ++pos;
    switch(c){
      case '+': push(TokKind::Plus); break;
      case '-': push(TokKind::Minus); break;
      case '*': push(TokKind::Star); break;
      case '/': push(TokKind::Slash); break;
      case '(': push(TokKind::LParen); break;
      case ')': push(TokKind::RParen); break;
      case ',': push(TokKind::Comma); break;
      case ';': push(TokKind::Semi); break;
      case '=': push(TokKind::Eq); break;
      case '^': push(TokKind::Caret); break;
      case '<':
        if(pos<(int)src.size() && src[pos]=='='){ ++pos; push(TokKind::Le); }
        else if(pos<(int)src.size() && src[pos]=='>'){ ++pos; push(TokKind::Ne); }
        else push(TokKind::Lt);
        break;
      case '>':
        if(pos<(int)src.size() && src[pos]=='='){ ++pos; push(TokKind::Ge); }
        else push(TokKind::Gt);
        break;
      default:
        // ignore unknowns
        break;
    }
  }

  out.push_back({TokKind::End, "", line});
  return out;
}

} // namespace pb
