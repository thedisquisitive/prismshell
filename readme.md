# PrismBASIC Shell

A retro-style BASIC shell that doubles as your terminal. Type line-numbered BASIC like it's 1983, or run modern commands via `/bin/sh`. Extend the shell using **mods written in PrismBASIC**. Customize your prompt with templates or mods.

> **Status:** Phase 2 complete. Production-ready with multidimensional arrays, hash maps, string manipulation, and classic BASIC flow control.

## Features

### Core Language
- **Classic line-numbered editor** with LIST, RUN, SAVE, LOAD
- **Direct execution mode** for immediate statement evaluation
- **User-defined subroutines** with parameters, return values, and recursion
- **Block control flow**: `IF/ELSEIF/ELSE/ENDIF`, `WHILE/WEND`
- **FOR/NEXT loops** with STEP support (including negative steps)
- **DATA/READ/RESTORE** for embedded program data
- **Boolean operators**: `AND`, `OR`, `NOT`

### Data Structures
- **1D arrays**: Fixed (`DIM arr[10]`) and dynamic (`DIM arr[]`)
- **Multidimensional arrays**: Up to any dimension (`DIM matrix[3,4]`, `DIM cube[2,3,4]`)
- **Dynamic expansion**: Auto-resize on access
- **Hash maps** (associative arrays): Key-value storage with `Map.*` functions
- **Array introspection**: `ARR.DIMS()`, `ARR.SIZE()`, `LEN()`

### String Processing
- **Substring extraction**: `LEFT()`, `RIGHT()`, `MID()`
- **Search**: `INSTR()` - find substring position
- **Case conversion**: `UCASE()`, `LCASE()`
- **Whitespace**: `TRIM()`, `LTRIM()`, `RTRIM()`
- **Character/ASCII**: `CHR()`, `ASC()`
- **Conversion**: `STR()`, `VAL()`

### System Integration
- **Rich builtins**: Filesystem (20+ operations), environment, path manipulation, RNG, time
- **Mod system** for extending the shell with BASIC plugins
- **Shell passthrough** for seamless Unix command integration
- **Customizable prompts** via templates or mods
- **Shebang support** for standalone BASIC scripts

## Quick Links

- [Installation](#installation) - Build requirements and setup
- [Getting Started](docs/Getting-Started.md) - Tutorial and examples
- [Language Reference](docs/Language.md) - Complete syntax documentation
- [Mods](docs/Mods.md) - Plugin system
- [Prompt Customization](docs/Prompt.md) - Customize your shell prompt
- [Troubleshooting](docs/Troubleshooting.md) - Common issues
- [Architecture](docs/Architecture.md) - Internal design
- [Roadmap](docs/Roadmap.md) - Future plans

## Installation

### Requirements

#### Build Dependencies
- **CMake** ≥ 3.20
- **C++17 compiler**:
  - GCC ≥ 7.0
  - Clang ≥ 5.0
  - MSVC ≥ 2017 (Visual Studio 15.0)
- **Make** or **Ninja** (build system backend)

#### Optional Dependencies
- **GNU Readline** (`libreadline-dev` or `readline-devel`)
  - Provides line editing, history, and command completion
  - Highly recommended for interactive use
  - Enable with `-DUSE_READLINE=ON` (default)

#### Runtime Dependencies
- **POSIX shell** (`/bin/sh`) for shell passthrough
  - Pre-installed on Linux, macOS, BSD
  - Available via WSL on Windows
  - Limited functionality on native Windows builds

### Platform-Specific Setup

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libreadline-dev git
```

**Fedora/RHEL/CentOS:**
```bash
sudo dnf install -y gcc-c++ cmake readline-devel make git
```

**Arch Linux:**
```bash
sudo pacman -S --needed base-devel cmake readline git
```

**macOS:**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if needed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake readline
```

**Windows (WSL - Recommended):**
```bash
# From Windows
wsl --install

# Inside WSL
sudo apt-get update
sudo apt-get install -y build-essential cmake libreadline-dev git
```

### Build from Source

#### Quick Build (Unix-like systems)

```bash
git clone https://github.com/yourusername/prismshell.git
cd prismshell
./build.sh
```

#### Manual Build

```bash
# Configure
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compile
cmake --build . -j

# Test
./prismshell ../test.bas

# Install (optional)
sudo cmake --install . --prefix /usr/local
# OR user-local:
cmake --install . --prefix ~/.local
```

### Build Options

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_READLINE=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DENABLE_WARNINGS=ON \
  -DENABLE_LTO=OFF \
  -DINSTALL_DOCS=ON
```

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel` |
| `USE_READLINE` | `ON` | Enable GNU Readline for line editing |
| `BUILD_SHARED_LIBS` | `OFF` | Build as shared library |
| `ENABLE_WARNINGS` | `ON` | Enable compiler warnings |
| `WARNINGS_AS_ERRORS` | `OFF` | Treat warnings as errors |
| `ENABLE_LTO` | `OFF` | Link Time Optimization |
| `INSTALL_DOCS` | `ON` | Install documentation and sample mods |

## Quick Examples

### Interactive Mode

```basic
PRINT "Hello, World!"
LET x = 42
PRINT "The answer: "; x

' Arrays
DIM nums[5]
LET nums[0] = 10
PRINT nums[0] + 20

' Hash maps
CALL Map.Set("user", "name", "Alice")
CALL Map.Get("user", "name")
PRINT "User: "; _

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
50 FOR i = 2 TO 19
60   LET fib[i] = fib[i-1] + fib[i-2]
70 NEXT i
80 FOR i = 0 TO 19
90   PRINT "fib("; i; ") = "; fib[i]
100 NEXT i
```

### Multidimensional Arrays

```basic
' 2D matrix operations
DIM matrix[3, 3]
FOR row = 0 TO 2
  FOR col = 0 TO 2
    LET matrix[row, col] = row * 3 + col + 1
  NEXT col
NEXT row

' 3D array
DIM cube[10, 10, 10]
LET cube[5, 5, 5] = 100
PRINT cube[5, 5, 5]
```

### Hash Maps

```basic
' Configuration storage
CALL Map.Set("config", "theme", "dark")
CALL Map.Set("config", "language", "en")
CALL Map.Set("config", "debug", "1")

' Retrieve values
CALL Map.Get("config", "theme")
PRINT "Theme: "; _

' Get all keys
CALL Map.Keys("config")
PRINT "Settings:"
PRINT _

' Word frequency counter
CALL Map.Get("freq", "hello", "0")
LET count = _
CALL VAL(count)
LET count = _ + 1
CALL STR(count)
CALL Map.Set("freq", "hello", _)
```

### String Manipulation

```basic
' Substring operations
CALL LEFT("Hello World", 5)
PRINT _  ' "Hello"

CALL MID("Hello World", 7, 5)
PRINT _  ' "World"

' Search
CALL INSTR("Hello World", "World")
PRINT "Position: "; _  ' 7

' Case conversion
CALL UCASE("hello")
PRINT _  ' "HELLO"

' Parse CSV
LET csv = "Alice,30,Engineer"
CALL INSTR(csv, ",")
LET pos = _
CALL LEFT(csv, pos - 1)
PRINT "Name: "; _
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

' Boolean operators
IF age > 18 AND age < 65 THEN
  PRINT "Working age"
ENDIF
```

### FOR/NEXT Loops

```basic
' Standard loop
FOR i = 1 TO 10
  PRINT i
NEXT i

' With step
FOR i = 0 TO 100 STEP 5
  PRINT i
NEXT i

' Countdown
FOR i = 10 TO 1 STEP -1
  PRINT "T-minus "; i
NEXT i
```

### DATA/READ/RESTORE

```basic
DATA 100, 200, 300
DATA "Alice", "Bob", "Charlie"

DIM values[3]
DIM names[3]

FOR i = 0 TO 2
  READ values[i]
NEXT i

FOR i = 0 TO 2
  READ names[i]
NEXT i

RESTORE  ' Reset to beginning
READ first
PRINT first  ' 100
```

### Shell Integration

Mix BASIC and shell commands seamlessly:

```bash
pbsh> PRINT "Files in current directory:"
pbsh> ls -la
pbsh> CALL FS.List(".")
pbsh> PRINT _
pbsh> grep "TODO" *.bas
```

### Shebang Scripts

```bash
#!/usr/bin/env prismshell
' hello.bas - Greet the user

PRINT "Welcome to PrismBASIC!"
CALL Env.Args()
PRINT "Arguments: "; _
```

```bash
chmod +x hello.bas
./hello.bas arg1 arg2 arg3
```

## Language Highlights

### Data Structures
- **1D arrays**: `DIM arr[10]` or `DIM arr[]` (dynamic)
- **Multidimensional**: `DIM matrix[3,4]`, `DIM cube[2,3,4]`
- **Hash maps**: `Map.Set()`, `Map.Get()`, `Map.Has()`, `Map.Keys()`, `Map.Size()`, `Map.Delete()`, `Map.Clear()`
- **Introspection**: `LEN()`, `ARR.DIMS()`, `ARR.SIZE()`

### Control Flow
- **Conditionals**: `IF...ELSEIF...ELSE...ENDIF`
- **Loops**: `WHILE...WEND`, `FOR...TO...STEP...NEXT`
- **Jumps**: `GOTO`, `GOSUB/RETURN`
- **Subroutines**: `SUB name(params)...END SUB`
- **Boolean**: `AND`, `OR`, `NOT`

### String Functions
- **Extract**: `LEFT()`, `RIGHT()`, `MID()`
- **Search**: `INSTR()`
- **Case**: `UCASE()`, `LCASE()`
- **Trim**: `TRIM()`, `LTRIM()`, `RTRIM()`
- **Convert**: `CHR()`, `ASC()`, `STR()`, `VAL()`

### Builtin Functions
- **Env.***: `Cwd()`, `Get()`, `Set()`, `Args()`, `Exit()`
- **FS.***: `Read()`, `Write()`, `Exists()`, `List()`, `Cd()`, `Mkdir()`, `Copy()`, `Move()`, `Touch()`, etc.
- **Path.***: `Join()`, `Dirname()`, `Basename()`, `Ext()`
- **RNG.***: `Seed()`, `Int()`, `Float()`
- **Time.***: `Now()`
- **Map.***: `Set()`, `Get()`, `Has()`, `Keys()`, `Size()`, `Delete()`, `Clear()`

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

```bash
pbsh> greet Alice
Hello, Alice!
```

**Autostart:** Control startup mods with `~/.prismrc`:

```ini
autoload_all = 0
autostart = prompt.bas
```

## Testing

Run the comprehensive test suite:

```bash
./prismshell test.bas
```

This validates:
- Arrays (1D and multidimensional)
- Hash maps
- String functions
- User SUBs with recursion
- Block IF/WHILE/FOR loops
- DATA/READ/RESTORE
- Boolean operators
- Math operations
- All 46 tests

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
├── mods/                  # Sample mods
├── test.bas               # Comprehensive test suite (46 tests)
├── CMakeLists.txt
├── build.sh              # Convenience build script
└── README.md
```

## Documentation

- **Man pages**: `man prismshell`, `man 7 prismshell-language`, `man 7 prismshell-mods`
- **Markdown docs**: See `docs/` directory
- **Examples**: Check `test.bas` for comprehensive feature demonstrations

## Current Status

**Phase 2 Complete**

- Core language: IF/WHILE/FOR, SUBs, arrays
- Multidimensional arrays with dynamic expansion
- Hash maps (associative arrays)
- Comprehensive string manipulation
- DATA/READ/RESTORE
- Boolean operators (AND/OR/NOT)
- 20+ filesystem operations
- Path manipulation helpers
- Random number generation
- Time functions

**Next Phase**: Local variables, error handling, advanced I/O

## Roadmap

**Phase 3** (Next):
- Local variables in SUBs
- Pass-by-reference for arrays
- Error handling (`ON ERROR`, `TRY/CATCH`)
- File handles and streaming I/O
- Regular expressions
- JSON parsing

**Phase 4+**:
- Bytecode VM with debugging
- Module system for C++ plugins
- Network I/O
- Database connectivity
- GUI toolkit integration

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

[To be determined]

## Acknowledgments

PrismBASIC is inspired by classic BASIC interpreters of the 1970s-80s, bringing retro computing aesthetics to modern Unix shells with contemporary language features like hash maps, multidimensional arrays, and comprehensive string processing.

---

**Made with ❤️ for retrocomputing enthusiasts and shell hackers**