#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <map>


namespace pb {


using Number = double;
using Value = std::variant<std::monostate, Number, std::string>;


std::string to_string(const Value& v);
bool truthy(const Value& v);


struct Error { int line{-1}; std::string msg; };


struct Result {
std::optional<Error> err;
};


// Simple string helpers
std::string trim(std::string s);
bool starts_with(const std::string& s, const std::string& p);
bool iequals(const std::string& a, const std::string& b);
std::vector<std::string> split_csv_like(const std::string& s);


} // namespace pb