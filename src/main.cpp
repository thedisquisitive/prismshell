#include "prismshell/interpreter.hpp"
#include <vector>
#include <string>


int main(int argc, char** argv){
pb::Interpreter I;
if(argc>1){
std::vector<std::string> args; for(int i=2;i<argc;++i) args.emplace_back(argv[i]);
return I.run_file(argv[1], args);
}
I.repl();
return 0;
}