# Troubleshooting

## “Not a file … cmake_install.cmake”
You skipped configure. Run:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cmake --install build --prefix ~/.local
```

## VS Code: `TokKind` unknown / missing types
Ensure header includes:
- `parser.hpp` includes `lexer.hpp`
- `#include <memory> <string> <vector> <optional>`
Point IntelliSense at your compile flags:
```
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
# VS Code clangd or:
"C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json"
```

## Linker errors for `Runtime::run_program(...)`
Header/impl signatures must match. The MVP provides:
- `Result run_program();`
- `Result run_program(int startLine);`

## “Parse error” on `IF ... THEN ...`
MVP supports **jump form only**:
```
IF X = 1 THEN 100
```
No inline THEN bodies or `ENDIF` blocks yet.

## Windows notes
- `/bin/sh` passthrough and `glob()` are POSIX; on Windows they’re stubbed. Use WSL for full behavior.
