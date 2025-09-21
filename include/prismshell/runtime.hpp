#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <random>

#include "prismshell/utils.hpp"
#include "prismshell/parser.hpp"  // ExprPtr, StmtPtr

namespace pb {

struct Runtime {
  std::map<std::string, Value> vars;   // variables (incl. PB_ARGV)
  std::map<int, std::string> program;  // line-numbered source
  Value lastCall;                       // `_`

  // Execution
  Result run_line_direct(const std::string& line, int lineNo=0);
  Result run_program();                 // run from beginning (existing behavior)
  Result run_program(int startLine);    // NEW: run starting at a specific line

  // Retro editor helpers
  void list();
  bool save(const std::string& path);
  bool load(const std::string& path);

  // Shell passthrough
  int sh_exec(const std::string& line);

  // Internals used by the interpreter/runtime
  Value  eval(const ExprPtr& e);
  Result exec(const StmtPtr& s, int* pc, std::vector<int>& gosubStack);

  // RNG state (per-runtime)
  std::mt19937_64 rng{};
  bool rng_seeded{false};
};

// Builtin CALL router
Value call_dispatch(Runtime& rt, const std::string& qname, const std::vector<Value>& args);

// (Optional) Mod registry API — useful if other translation units need it
bool mod_has(const std::string& name);
int  mod_run(const std::string& name, const std::vector<std::string>& args, Runtime& parent);



} // namespace pb
