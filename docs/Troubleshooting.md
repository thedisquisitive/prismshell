# ========================================
# Troubleshooting.md - COMPLETE REPLACEMENT
# ========================================

# Troubleshooting

## Build Issues

### "Not a file … cmake_install.cmake"
You skipped the configure step. Run:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cmake --install build --prefix ~/.local
```

### "CMake version too old"
Install CMake ≥ 3.20:
```bash
# Ubuntu/Debian
sudo snap install cmake --classic

# macOS
brew install cmake

# Or from source: https://cmake.org/download/
```

### "Readline not found"
```bash
# Debian/Ubuntu
sudo apt-get install libreadline-dev

# Fedora/RHEL
sudo dnf install readline-devel

# macOS
brew install readline

# Or build without readline
cmake -S . -B build -DUSE_READLINE=OFF
```

## IDE/Editor Issues

### VS Code: `TokKind` unknown / missing types
Ensure header includes:
- `parser.hpp` includes `lexer.hpp`
- `#include <memory> <string> <vector> <optional>`

Point IntelliSense at your compile flags:
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
```

Then in VS Code settings:
```json
"C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json"
```

### Linker errors for `Runtime::run_program(...)`
Header/implementation signatures must match. The current API provides:
- `Result run_program();`
- `Result run_program(int startLine);`

## Language/Runtime Issues

### "Parse error" on valid-looking BASIC
Check that you're using supported syntax:
- Block IF requires `ENDIF` (not `END IF` with space)
- FOR loops require `NEXT` (optional variable name)
- Array indices use brackets: `arr[i]` not `arr(i)`
- Multidimensional arrays use commas: `matrix[i, j]`

### Array dimension mismatch errors
Ensure you access arrays with the correct number of indices:
```basic
DIM matrix[3, 4]
LET matrix[1, 2] = 42  ' Correct: 2 indices
LET matrix[1] = 42     ' Error: only 1 index
```

### DATA/READ out of sync
Remember:
- DATA statements accumulate in program order
- Each READ consumes one value per variable
- Use RESTORE to reset to beginning
- Reading past end of data causes "Out of DATA" error

### FOR loop not executing
Check step direction:
```basic
FOR i = 10 TO 1        ' Won't execute (step defaults to +1)
FOR i = 10 TO 1 STEP -1  ' Correct
```

## Platform-Specific Issues

### Windows notes
- `/bin/sh` passthrough and `glob()` are POSIX; on native Windows they're stubbed
- Use WSL for full functionality
- File paths: use forward slashes or double backslashes

### macOS notes
- Ensure Xcode Command Line Tools installed: `xcode-select --install`
- Readline may need explicit linking if installed via Homebrew

## Runtime Errors

### "Interrupted (Ctrl-C)" during program execution
This is normal behavior. Ctrl-C stops running programs.
- In REPL: press Ctrl-C to cancel current input
- In programs: press Ctrl-C to halt execution

### Stack overflow with recursive SUBs
Current implementation has no recursion limit. Ensure:
- Base case is reachable
- Parameters actually change between calls
- Consider iterative approach for deep recursion

### Array index out of bounds
Dynamic arrays auto-expand, but negative indices are invalid:
```basic
DIM arr[]
LET arr[-1] = 42  ' Error: negative index
LET arr[1000] = 42  ' OK: auto-expands
```

## Performance Issues

### Slow program execution
Current AST interpreter has no optimization. Consider:
- Reducing loop iterations for testing
- Avoiding deep nesting
- Phase 6 will add bytecode compilation

### Large array allocation fails
Multidimensional arrays can consume significant memory:
```basic
DIM huge[1000, 1000, 1000]  ' 1 billion elements!
```
Use dynamic arrays or smaller dimensions when possible.

## Getting Help

If you encounter issues not covered here:
1. Check the test suite: `./prismshell test.bas`
2. Review documentation: `docs/` directory
3. Examine example programs in `mods/`
4. File an issue with:
   - PrismBASIC version
   - Operating system
   - Minimal reproduction case
   - Expected vs actual behavior

---