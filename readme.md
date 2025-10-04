# PrismBASIC Shell

A retro-style BASIC shell that doubles as your terminal. Type line-numbered BASIC like it's 1983, or run modern commands via `/bin/sh`. Extend the shell using **mods written in PrismBASIC**. Customize your prompt with templates or mods.

> **Status:** Phase 1 complete. Full-featured with arrays, user SUBs, and block control flow.

## ✨ Features

- **Classic line-numbered editor** with LIST, RUN, SAVE, LOAD
- **Direct execution mode** for immediate statement evaluation
- **Arrays** with fixed and dynamic sizing (`DIM name[size]` and `DIM name[]`)
- **User-defined subroutines** with parameters, return values, and recursion
- **Block control flow**: `IF/ELSEIF/ELSE/ENDIF`, `WHILE/WEND`
- **Rich builtins**: filesystem, environment, path manipulation, random numbers, time
- **Mod system** for extending the shell with BASIC plugins
- **Shell passthrough** for seamless Unix command integration
- **Customizable prompts** via templates or mods
- **Shebang support** for standalone BASIC scripts

## Quick Links

- [Installation](docs/Installation.md)
- [Getting Started](docs/Getting-Started.md)
- [Language Reference](docs/Language.md)
- [Mods (PrismBASIC plugins)](docs/Mods.md)
- [Prompt Customization](docs/Prompt.md)
- [Troubleshooting](docs/Troubleshooting.md)
- [Architecture Overview](docs/Architecture.md)
- [Roadmap](docs/Roadmap.md)

## Quickstart

```bash
# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j

# Run
./prismshell              # interactive
./prismshell demo.bas     # run a program
./prismshell test.bas     # comprehensive test suite
```

Or use the convenient build script:

```bash
./build.sh
```

## Quick Examples

### Interactive Mode

```basic
PRINT "Hello, World!"
LET x = 42
PRINT "The answer: "; x

' Arrays
DIM nums[5]
LET nums[0] = 10
LET nums[1] = 20
PRINT nums[0] + nums[1]

' Builtins
CALL Env.Cwd()
PRINT "Current directory: "; _
```

### Line-Numbered Programs

```basic
10 PRINT "Fibonacci Calculator"
20 DIM fib[20]
30 LET fib[0] = 0
40 LET fib[1] = 1
50 LET i = 2
60 WHILE i < 20
70   LET fib[i] = fib[i-1] + fib[i-2]
80   LET i = i + 1
90 WEND
100 LET i = 0
110 WHILE i < 20
120   PRINT "fib("; i; ") = "; fib[i]
130   LET i = i + 1
140 WEND
```

### User Subroutines

```basic
SUB Factorial(n)
  IF n <= 1 THEN
    LET _ = 1
  ELSE
    CALL Factorial(n - 1)
    LET _ = n * _
  ENDIF
END SUB

CALL Factorial(5)
PRINT "5! = "; _
```

### Block Control Flow

```basic
LET age = 25
IF age < 13 THEN
  PRINT "Child"
ELSEIF age < 20 THEN
  PRINT "Teenager"
ELSEIF age < 65 THEN
  PRINT "Adult"
ELSE
  PRINT "Senior"
ENDIF
```

### Shell Integration

Mix BASIC and shell commands seamlessly:

```
pbsh> PRINT "Files in current directory:"
pbsh> ls -la
pbsh> CALL FS.List(".")
pbsh> PRINT _
```

### Shebang Scripts

Create executable BASIC programs:

```basic
#!/usr/bin/env prismshell
' hello.bas - A simple greeting script

PRINT "Welcome to PrismBASIC!"
CALL Env.Args()
PRINT "Arguments: "; _
```

```bash
chmod +x hello.bas
./hello.bas arg1 arg2 arg3
```

## Language Highlights

### Core Features
- **Variables**: `LET name = expr`
- **Arrays**: `DIM name[10]` or `DIM name[]` (dynamic)
- **I/O**: `PRINT`, `INPUT`, `TTY.*` functions
- **Math**: `+ - * / ^` with proper precedence
- **Strings**: Concatenation with `+`
- **Comparisons**: `== != < <= > >=`

### Control Flow
- **Conditionals**: `IF...ELSEIF...ELSE...ENDIF`
- **Loops**: `WHILE...WEND`
- **Jumps**: `GOTO`, `GOSUB/RETURN`
- **Subroutines**: `SUB name(params)...END SUB`

### Builtin Functions
- **Env.***: `Cwd()`, `Get()`, `Set()`, `Args()`, `Exit()`
- **FS.***: `Read()`, `Write()`, `Exists()`, `List()`, `Cd()`, `Mkdir()`, etc.
- **Path.***: `Join()`, `Dirname()`, `Basename()`, `Ext()`
- **RNG.***: `Seed()`, `Int()`, `Float()`
- **Time.***: `Now()`
- **Array**: `LEN(string)`, `LEN("arrayname")`

## Mod System

Extend the shell with BASIC plugins:

```basic
' mods/greet.bas
10 CALL Mod.Register("greet", 100)
20 END

100 IF PB_ARGC = 0 THEN 1000
110 PRINT "Hello, "; PB_ARG1; "!"
120 END

1000 PRINT "usage: greet <name>"
1010 END
```

Use in the shell:

```
pbsh> greet Alice
Hello, Alice!
```

**Autostart:** Control startup mods with `~/.prismrc`:

```ini
autoload_all = 0
autostart = prompt.bas
```

## Installation

### Requirements
- CMake ≥ 3.20
- C++17 compiler (gcc, clang, MSVC)
- Optional: GNU Readline for line editing

### Build & Install

```bash
# Configure and build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Install (optional)
sudo cmake --install build --prefix /usr/local

# Or use the build script
./build.sh
```

### Build Options

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_READLINE=ON \
  -DENABLE_WARNINGS=ON \
  -DWARNINGS_AS_ERRORS=OFF \
  -DENABLE_LTO=ON \
  -DINSTALL_DOCS=ON
```

## Testing

Run the comprehensive test suite:

```bash
./prismshell test.bas
```

This validates:
- Basic arrays (fixed and dynamic)
- Array operations with loops
- String arrays
- User SUBs with parameters
- Recursive SUBs
- SUB calling SUB
- Block IF/ELSEIF/ELSE
- Nested WHILE loops
- Math operations and string concatenation

## Project Structure

```
prismshell/
├── src/
│   ├── main.cpp           # Entry point
│   ├── interpreter.cpp    # REPL and file execution
│   ├── runtime.cpp        # Expression eval, statement execution
│   ├── parser.cpp         # AST construction
│   ├── lexer.cpp          # Tokenization
│   └── utils.cpp          # Helper functions
├── include/prismshell/
│   ├── interpreter.hpp
│   ├── runtime.hpp
│   ├── parser.hpp
│   ├── lexer.hpp
│   └── utils.hpp
├── docs/                  # Markdown documentation
├── man/                   # Man pages
│   ├── man1/             # prismshell(1)
│   └── man7/             # language, mods, prompt references
├── mods/                  # Sample mods (installed with docs)
├── test.bas               # Comprehensive test suite
├── CMakeLists.txt
├── build.sh              # Convenience build script
└── README.md
```

## Documentation

- **Man pages**: `man prismshell`, `man 7 prismshell-language`, `man 7 prismshell-mods`
- **Markdown docs**: See `docs/` directory
- **Examples**: Check `test.bas` for comprehensive feature demonstrations

## Current Status

**Phase 1 Complete** ✅

- ✅ Arrays (fixed and dynamic)
- ✅ User-defined SUBs with recursion
- ✅ Block IF/ELSEIF/ELSE/ENDIF
- ✅ WHILE/WEND loops
- ✅ Comprehensive filesystem operations
- ✅ Path manipulation helpers
- ✅ Random number generation
- ✅ Time functions
- ✅ LEN() for strings and arrays

## Roadmap

**Phase 2** (Next):
- `FOR/NEXT` loops with step
- `DATA/READ/RESTORE`
- Multi-dimensional arrays
- String manipulation functions

**Phase 3+**:
- Local variables and pass-by-reference
- Error handling (`ON ERROR`, `TRY/CATCH`)
- Bytecode VM with debugging
- Module system for C++ plugins
- PrismFS versioning backends

See [Roadmap.md](docs/Roadmap.md) for details.

## Contributing

Contributions welcome! Areas of interest:

- Additional builtin functions
- More sample mods
- Test coverage expansion
- Documentation improvements
- Platform-specific optimizations
- Bug fixes and performance enhancements

## License

[To be determined - please specify your license]

## Acknowledgments

PrismBASIC is inspired by classic BASIC interpreters of the 1970s-80s, bringing retro computing aesthetics to modern Unix shells with contemporary language features.

---

**Made with ❤️ for retrocomputing enthusiasts and shell hackers**