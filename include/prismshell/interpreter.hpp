#pragma once
#include <string>
#include "prismshell/runtime.hpp"


namespace pb {


struct Interpreter {
Runtime rt;
void repl(char const* prompt="pbsh> ");
int run_file(const std::string& path, const std::vector<std::string>& args);
};


} // namespace pb