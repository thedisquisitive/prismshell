# Roadmap

## ✅ Phase 1 Complete (Current Release)
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

## Phase 2: Enhanced Data Structures
- `FOR/NEXT` loops with step support
- `DATA/READ/RESTORE` for embedded data
- Multi-dimensional arrays (`DIM arr[x,y]`)
- String manipulation functions (LEFT, RIGHT, MID, INSTR, etc.)
- Better argv handling (parse PB_ARGV as proper array value)
- Hash maps / associative arrays

## Phase 3: Advanced Features
- Local variables in SUBs (explicit `LOCAL` keyword)
- Pass-by-reference for arrays to SUBs
- Error handling (`ON ERROR`, `TRY/CATCH`)
- File handles and streaming I/O
- Regular expression support
- JSON parsing/generation builtins

## Phase 4: Development Tools
- Colors in prompt (Readline-safe ANSI wrappers)
- Line editor improvements (syntax highlighting, auto-completion)
- Debug mode with breakpoints and step execution
- `TRACE ON/OFF` for execution logging
- Performance profiling tools
- Unit testing framework for BASIC programs

## Phase 5: Advanced Runtime
- Bytecode compilation + small VM (currently AST interpreter)
- JIT optimization for hot loops
- Coroutines / cooperative multitasking
- Module system for CALL providers (C++ plugins via dlopen/LoadLibrary)
- Foreign function interface (FFI) for calling C libraries
- WebAssembly compilation target

## Phase 6: PrismFS & Versioning
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
- Tail call optimization for recursive SUBs
- Constant folding and expression optimization
- String interning for frequently used literals
- Memory pool allocators for runtime objects

---

**Current Status:** Phase 1 complete. The MVP now includes full block control flow, user-defined subroutines, arrays with dynamic sizing, and a comprehensive set of filesystem, path, and utility builtins. The shell is production-ready for scripting and interactive use.

**Next Milestone:** Phase 2 focus on `FOR/NEXT` loops and `DATA/READ/RESTORE` for classic BASIC data handling patterns.