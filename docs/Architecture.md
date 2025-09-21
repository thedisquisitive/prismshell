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

## Control Flow (MVP)

- IF is jump-only (`IF cond THEN <line>`).
- `GOSUB` uses a stack in `Runtime::exec(...)`.
- `END` sets `pc` to a sentinel to terminate.

## Extensibility

- Add new CALLs in `call_dispatch`.
- Add grammar in `parser.cpp` (remember precedence, unary ops, comments).
- Add subs/blocks later and map to runtime calls or a tiny VM.
