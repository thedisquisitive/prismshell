#include "prismshell/lexer.hpp"
#include "prismshell/utils.hpp"
#include <cctype>


namespace pb {


static bool isid0(char c){ return std::isalpha((unsigned char)c) || c=='_' || c=='.'; }
static bool isid(char c){ return std::isalnum((unsigned char)c) || c=='_' || c=='.'; }


std::vector<Token> Lexer::lex(){
std::vector<Token> out;
auto push=[&](TokKind k, std::string t=""){ out.push_back({k,std::move(t),line}); };
auto skipws=[&]{ while(pos<(int)src.size()){ char c=src[pos]; if(c=='\t' || c==' ') { ++pos; } else break; } };
skipws();
while(pos<(int)src.size()){
char c=src[pos];
if(c=='\t' || c==' ') { ++pos; continue; }
if(c=='"'){
std::string s; ++pos; while(pos<(int)src.size() && src[pos]!='"'){ s.push_back(src[pos++]); }
if(pos<(int)src.size() && src[pos]=='"') ++pos; push(TokKind::Str, s); continue;
}
if(std::isdigit((unsigned char)c)){
int start=pos; while(pos<(int)src.size() && (std::isdigit((unsigned char)src[pos])||src[pos]=='.')) ++pos; push(TokKind::Num, src.substr(start,pos-start)); continue;
}
if(isid0(c)){
int start=pos; while(pos<(int)src.size() && isid(src[pos])) ++pos; std::string id=src.substr(start,pos-start);
std::string up=id; for(char& ch:up) ch=std::toupper((unsigned char)ch);
if(up=="LET") push(TokKind::Let);
else if(up=="PRINT") push(TokKind::Print);
else if(up=="INPUT") push(TokKind::Input);
else if(up=="IF") push(TokKind::If);
else if(up=="THEN") push(TokKind::Then);
else if(up=="ELSE") push(TokKind::Else);
else if(up=="END") push(TokKind::EndTok);
else if(up=="REM") push(TokKind::Rem);
else if(up=="GOTO") push(TokKind::Goto);
else if(up=="GOSUB") push(TokKind::Gosub);
else if(up=="RETURN") push(TokKind::ReturnTok);
else if(up=="CALL") push(TokKind::Call);
else push(TokKind::Id, id);
continue;
}
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
case '<': if(pos<(int)src.size() && src[pos]=='='){ ++pos; push(TokKind::Le); }
else if(pos<(int)src.size() && src[pos]=='>'){ ++pos; push(TokKind::Ne); }
else push(TokKind::Lt); break;
case '>': if(pos<(int)src.size() && src[pos]=='='){ ++pos; push(TokKind::Ge); } else push(TokKind::Gt); break;
default: break; // ignore unknowns
}
}
out.push_back({TokKind::End, "", line});
return out;
}


} // namespace pb