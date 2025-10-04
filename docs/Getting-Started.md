# Getting Started

## Direct Mode

Type BASIC statements and they execute immediately:

```basic
PRINT "Hello, World!"
CALL Env.Cwd()
PRINT _
LET x = 42
PRINT "The answer is "; x
```

The special variable `_` holds the result of the last `CALL`.

## Line-numbered Editor

Classic BASIC with line numbers for program storage:

```basic
10 PRINT "PrismBASIC v0.1 — type HELP"
20 INPUT name
30 PRINT "Hello, "; name; "!"
40 CALL Env.Cwd()
50 PRINT "Current directory: "; _
LIST
RUN
SAVE hello.bas
NEW
LOAD hello.bas
```

### Editor Commands
- `LIST` — display the current program
- `RUN` — execute the line-numbered program
- `SAVE <file>` — persist program to disk
- `LOAD <file>` — restore program from disk
- `NEW` — clear the program
- Typing a bare line number (e.g., `30`) deletes that line

## Arrays

PrismBASIC supports both fixed-size and dynamic arrays:

```basic
' Fixed-size array
DIM numbers[5]
LET numbers[0] = 10
LET numbers[1] = 20
PRINT numbers[0]

' Dynamic array (auto-expands)
DIM data[]
LET data[0] = 100
LET data[10] = 1000  ' Automatically resizes
CALL LEN("data")
PRINT "Array size: "; _
```

## Block Control Flow

### IF/ELSEIF/ELSE/ENDIF

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

### WHILE/WEND Loops

```basic
LET i = 1
WHILE i <= 5
  PRINT "Count: "; i
  LET i = i + 1
WEND
```

## User-Defined Subroutines

Define reusable procedures with parameters and return values:

```basic
SUB Double(x)
  LET _ = x * 2
END SUB

SUB Greet(name)
  PRINT "Hello, "; name; "!"
END SUB

SUB Factorial(n)
  IF n <= 1 THEN
    LET _ = 1
  ELSE
    CALL Factorial(n - 1)
    LET _ = n * _
  ENDIF
END SUB

CALL Double(21)
PRINT "Double(21) = "; _

CALL Greet("Alice")

CALL Factorial(5)
PRINT "5! = "; _
```

Key points:
- Use `LET _ = value` to set the return value
- SUBs can call other SUBs and be recursive
- Parameters are passed by value

## Shebang Scripts

Make standalone executable BASIC programs:

```bash
#!/usr/bin/env prismshell
PRINT "Script arguments:"
CALL Env.Args()
PRINT _
```

```bash
chmod +x script.bas
./script.bas arg1 arg2 arg3
```

## Shell Passthrough

Lines that don't parse as BASIC are executed by `/bin/sh -lc` (on POSIX systems):

```bash
echo "Mixing shell and BASIC!"
ls -la
grep -r "TODO" src/
cd /tmp
CALL Env.Cwd()
PRINT "Now in: "; _
```

Quoted arguments are supported with proper shell tokenization:

```bash
echo "hello world"
grep "search term" file.txt
find . -name "*.bas"
```

## Builtins Example

```basic
' File operations
CALL FS.Write("test.txt", "Hello, world!")
CALL FS.Read("test.txt")
PRINT "File contents: "; _

' Random numbers
CALL RNG.Seed()
CALL RNG.Int(1, 100)
PRINT "Random number: "; _

' Time
CALL Time.Now()
PRINT "Current time: "; _

' Environment
CALL Env.Get("HOME")
PRINT "Home directory: "; _
```

## Autostart Mods

Control which mods run at startup using `~/.prismrc`:

```ini
# Only run specific mods
autoload_all = 0
autostart = prompt.bas

# Or run all discovered mods
# autoload_all = 1
```

If no `.prismrc` exists, the default behavior is to run `prompt.bas` only.

## Complete Example Program

Here's a more substantial example combining multiple features:

```basic
#!/usr/bin/env prismshell
' Fibonacci calculator with array caching

10 PRINT "Fibonacci Calculator"
20 PRINT "==================="
30 PRINT ""

' Initialize array
40 DIM fib[50]
50 LET fib[0] = 0
60 LET fib[1] = 1

' Calculate series
70 LET i = 2
80 WHILE i < 20
90   LET fib[i] = fib[i-1] + fib[i-2]
100  LET i = i + 1
110 WEND

' Display results
120 LET i = 0
130 WHILE i < 20
140   PRINT "fib("; i; ") = "; fib[i]
150   LET i = i + 1
160 WEND

170 END
```

## Next Steps

- Read the [Language Reference](Language.md) for complete syntax
- Learn about [Mods](Mods.md) for extending the shell
- Customize your [Prompt](Prompt.md)
- Check [Troubleshooting](Troubleshooting.md) if you encounter issues