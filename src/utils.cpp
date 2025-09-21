#include "prismshell/utils.hpp"
#include <cctype>
#include <sstream>
#include <iomanip>
#include <regex>


namespace pb {


std::string trim(std::string s){
size_t a=0,b=s.size();
while(a<b && std::isspace((unsigned char)s[a])) ++a;
while(b>a && std::isspace((unsigned char)s[b-1])) --b;
return s.substr(a,b-a);
}


bool starts_with(const std::string& s, const std::string& p){
return s.rfind(p,0)==0;
}


bool iequals(const std::string& a, const std::string& b){
if(a.size()!=b.size()) return false; for(size_t i=0;i<a.size();++i){ if(std::tolower((unsigned char)a[i])!=std::tolower((unsigned char)b[i])) return false; } return true;
}


std::vector<std::string> split_csv_like(const std::string& s){
std::vector<std::string> out; std::string cur; bool inq=false; for(size_t i=0;i<s.size();++i){ char c=s[i]; if(c=='"'){ inq=!inq; cur.push_back(c); }
else if(c==',' && !inq){ out.push_back(trim(cur)); cur.clear(); }
else cur.push_back(c); }
if(!cur.empty()) out.push_back(trim(cur)); return out;
}


std::string to_string(const Value& v){
if(std::holds_alternative<std::monostate>(v)) return "";
if(std::holds_alternative<Number>(v)){
std::ostringstream oss; oss<<std::setprecision(15)<<std::get<Number>(v); return oss.str();
}
return std::get<std::string>(v);
}


bool truthy(const Value& v){
if(std::holds_alternative<Number>(v)) return std::get<Number>(v)!=0.0;
if(std::holds_alternative<std::string>(v)) return !std::get<std::string>(v).empty();
return false;
}


} // namespace pb