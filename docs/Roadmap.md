# Roadmap

## ✅ Phase 1 Complete
- ✅ Block control flow: `IF...ELSEIF...ELSE...ENDIF`, `WHILE/WEND`
- ✅ User `SUB ... END SUB` with parameters and return values
- ✅ Arrays: `DIM name[size]` and `DIM name[]` with dynamic expansion
- ✅ Array indexing in expressions and assignments
- ✅ `LEN()` function for strings and arrays
- ✅ Recursive SUBs with proper scoping
- ✅ Enhanced filesystem operations (cd, mkdir, rmdir, copy, move, touch, etc.)
- ✅ Path manipulation helpers (join, dirname, basename, ext)
- ✅ Random number generation (`RNG.*` namespace)
- ✅ Time functions (`Time.Now()`)
- ✅ Exponentiation operator (`^`)

## ✅ Phase 2 Complete (Current Release)
- ✅ `FOR/NEXT` loops with STEP support (including negative steps)
- ✅ Nested FOR loops
- ✅ `DATA/READ/RESTORE` for embedded data
- ✅ Multi-dimensional arrays (`DIM arr[x,y]`, `DIM arr[x,y,z]`, etc.)
- ✅ Dynamic multidimensional arrays (`DIM arr[, ]`)
- ✅ Mixed static/dynamic dimensions (`DIM arr[10, ]`)
- ✅ Array dimension queries (`ARR.DIMS()`, `ARR.SIZE()`)
- ✅ Row-major storage model (C-style)
- ✅ READ into array elements (`READ arr[i]`, `READ matrix[i,j]`)

## Phase 3: String Manipulation & Enhanced I/O
- String manipulation functions (LEFT, RIGHT, MID, INSTR, CHR, ASC)
- String comparison functions (STRCMP, UCASE, LCASE)
- String trimming and splitting (TRIM, LTRIM, RTRIM, SPLIT)
- STR/VAL for number↔string conversion
- Better argv handling (parse PB_ARGV as proper array value)
- Hash maps / associative arrays
- File handles and streaming I/O

## Phase 4: Advanced Language Features
- Local variables in SUBs (explicit `LOCAL` keyword)
- Pass-by-reference for arrays to SUBs
- Error handling (`ON ERROR`, `TRY/CATCH`)
- Regular expression support
- JSON parsing/generation builtins
- SELECT CASE statements
- DO...LOOP variants (DO WHILE, DO UNTIL, LOOP WHILE, LOOP UNTIL)

## Phase 5: Development Tools
- Colors in prompt (Readline-safe ANSI wrappers)
- Line editor improvements (syntax highlighting, auto-completion)
- Debug mode with breakpoints and step execution
- `TRACE ON/OFF` for execution logging
- Performance profiling tools
- Unit testing framework for BASIC programs
- Interactive debugger commands (STEP, CONTINUE, BREAK, WATCH)

## Phase 6: Advanced Runtime
- Bytecode compilation + small VM (currently AST interpreter)
- JIT optimization for hot loops
- Coroutines / cooperative multitasking
- Module system for CALL providers (C++ plugins via dlopen/LoadLibrary)
- Foreign function interface (FFI) for calling C libraries
- WebAssembly compilation target
- Tail call optimization for recursive SUBs

## Phase 7: PrismFS & Versioning
- Real PrismFS backends (`MOUNT.ADD`, `SNAPSHOT`, `BRANCH`)
- Git-like version control operations
- Overlay filesystem support
- Snapshot diffing and rollback
- Branch merging and conflict resolution

## Long-term Vision
- Network I/O (sockets, HTTP client/server)
- Database connectivity (SQLite, PostgreSQL)
- GUI toolkit integration (optional ncurses/SDL builds)
- Package manager for distributing mods and libraries
- Language server protocol (LSP) implementation for IDE integration
- Comprehensive test suite and fuzzing for parser/runtime
- Cross-platform improvements (better Windows support without WSL)

## Community Features
- Online mod repository
- Interactive tutorial system
- Example program gallery
- Community-contributed builtin library
- Documentation generator from source comments

## Performance & Optimization
- Lazy evaluation strategies
- Constant folding and expression optimization
- String interning for frequently used literals
- Memory pool allocators for runtime objects

---

**Current Status:** Phase 2 complete. The implementation now includes full block control flow, user-defined subroutines, FOR/NEXT loops, DATA/READ/RESTORE, and multidimensional arrays with both fixed and dynamic sizing. The shell is production-ready for scripting and interactive use with comprehensive array support.

**Next Milestone:** Phase 3 focus on string manipulation functions and enhanced I/O capabilities.

---
