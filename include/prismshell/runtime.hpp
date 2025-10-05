#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <random>

#include "prismshell/utils.hpp"
#include "prismshell/parser.hpp"  // ExprPtr, StmtPtr

namespace pb {

// User-defined SUB structure
struct SubDefinition {
  std::vector<std::string> params;
  std::map<int, std::string> body;  // line-numbered body
  int entryLine;
};

// NEW: FOR loop state tracking
struct ForLoopState {
  std::string var;      // loop variable name
  double end;           // end value
  double step;          // step increment
  int loopStartLine;    // first line after FOR statement
};

// Multidimensional array support
struct ArrayData {
  std::vector<size_t> dimensions;  // e.g., [3, 4] for matrix[3,4]
  std::vector<Value> data;         // flattened storage (row-major)
  bool isDynamic{false};           // true if any dimension can auto-expand
  
  // Convert multi-index to linear index (row-major order)
  size_t linearIndex(const std::vector<int>& indices) const;
  
  // Auto-expand for dynamic arrays
  void ensureCapacity(const std::vector<int>& indices);
};

struct Runtime {
  std::map<std::string, Value> vars;   // variables (incl. PB_ARGV)
  std::map<int, std::string> program;  // line-numbered source
  Value lastCall;                       // `_`

  std::map<std::string, ArrayData> arrays;
  std::map<std::string, SubDefinition> subs;         // user SUBs
  std::map<std::string, std::map<std::string, Value>> maps; // Hashmaps

  std::vector<ForLoopState> forStack;

  std::vector<Value> dataPool;    // all DATA values in program order
  size_t dataPointer{0};          // current READ position

  // Execution
  Result run_line_direct(const std::string& line, int lineNo=0);
  Result run_program();                 // run from beginning
  Result run_program(int startLine);    // run starting at a specific line

  // Retro editor helpers
  void list();
  bool save(const std::string& path);
  bool load(const std::string& path);

  // Shell passthrough
  int sh_exec(const std::string& line);

  // Internals used by the interpreter/runtime
  Value  eval(const ExprPtr& e);
  Result exec(const StmtPtr& s, int* pc, std::vector<int>& gosubStack);
  void buildDataPool();

  // RNG state (per-runtime)
  std::mt19937_64 rng{};
  bool rng_seeded{false};
};

// Builtin CALL router
Value call_dispatch(Runtime& rt, const std::string& qname, const std::vector<Value>& args);

// User SUB execution
Value call_user_sub(Runtime& rt, const SubDefinition& sub, const std::vector<Value>& args);

// Mod registry API
bool mod_has(const std::string& name);
int  mod_run(const std::string& name, const std::vector<std::string>& args, Runtime& parent);
void extract_subs(Runtime& rt);

} // namespace pb