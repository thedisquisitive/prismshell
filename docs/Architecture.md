# ========================================
# Architecture.md - COMPLETE REPLACEMENT
# ========================================

# Architecture Overview

## High-Level

- **Lexer/Parser**: tokenizes and parses BASIC into simple `Stmt`/`Expr` trees.
- **Runtime**: evaluates expressions, executes statements, and routes `CALL` to builtins.
- **Interpreter**: REPL and editor (numbered lines), shell passthrough, mod autoload, prompt building.

## Key Paths

- `CALL` dispatch: `call_dispatch(Runtime&, qname, args)` in `src/runtime.cpp`
- Program storage: `Runtime::program` (map of `line -> source`)
- Direct mode: `Runtime::run_line_direct(...)`
- Program mode: `Runtime::run_program(...)`
- Mod registry: in-memory map (`Mod.Register("name", entryLine)`)
- Prompt: Either `prompt` mod output or template expansion in the interpreter.

## Data Structures

### Arrays
- **ArrayData**: Multidimensional arrays with row-major storage
  - `dimensions`: vector of dimension sizes
  - `data`: flattened storage (last index varies fastest)
  - `isDynamic`: auto-expansion flag
  - Methods: `linearIndex()`, `ensureCapacity()`

### Control Flow
- **IF blocks**: `IF...ELSEIF...ELSE...ENDIF` with nesting support
- **WHILE loops**: `WHILE...WEND` with nesting
- **FOR loops**: `FOR...NEXT` with STEP, tracked via `ForLoopState` stack
- **GOSUB**: Return address stack in `Runtime::exec()`
- **SUBs**: Extracted to `Runtime::subs` map, executed in child runtime

### DATA Pool
- `Runtime::dataPool`: All DATA values in program order
- `Runtime::dataPointer`: Current READ position
- Built during `Runtime::buildDataPool()` before execution

## Control Flow Implementation

- IF jump form: `IF cond THEN <line>` (single-line)
- IF block form: `IF...ELSEIF...ELSE...ENDIF` (multi-line, nesting supported)
- FOR/NEXT: Loop state tracked in `Runtime::forStack`
- WHILE/WEND: Forward scan to find matching WEND, backward jump on WEND
- `GOSUB` uses return address stack in `Runtime::exec(...)`
- `END` sets `pc` to a sentinel to terminate

## Extensibility

- Add new CALLs in `call_dispatch` (`runtime.cpp`)
- Add grammar in `parser.cpp` (remember precedence, unary ops, comments)
- Add statement types to `Stmt` enum and implement in `Runtime::exec()`
- Arrays are now multidimensional via `ArrayData` structure

---