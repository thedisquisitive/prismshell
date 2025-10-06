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
10 PRINT "PrismBASIC v0.1 – type HELP"
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
- `LIST` – display the current program
- `RUN` – execute the line-numbered program
- `SAVE <file>` – persist program to disk
- `LOAD <file>` – restore program from disk
- `NEW` – clear the program
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

## Multidimensional Arrays

Work with matrices, grids, and higher-dimensional data:

```basic
' 2D matrix (3 rows × 4 columns)
DIM matrix[3, 4]
LET matrix[0, 0] = 1
LET matrix[2, 3] = 99
PRINT matrix[2, 3]

' Dynamic 2D array (auto-expands)
DIM grid[, ]
LET grid[100, 200] = 42
CALL ARR.SIZE("grid", 0)
PRINT "Rows: "; _  ' 101
CALL ARR.SIZE("grid", 1)
PRINT "Cols: "; _  ' 201

' 3D array
DIM cube[10, 10, 10]
LET cube[5, 5, 5] = 100
PRINT cube[5, 5, 5]

' Mixed: fixed first dimension, dynamic second
DIM hybrid[10, ]
LET hybrid[0, 1000] = 777  ' Row 0, col 1000

' Query dimensions
CALL ARR.DIMS("matrix")
PRINT "Dimensions: "; _  ' 2

CALL ARR.SIZE("matrix", 0)
PRINT "Rows: "; _  ' 3

CALL ARR.SIZE("matrix", 1)
PRINT "Cols: "; _  ' 4

' Matrix operations with FOR loops
DIM result[3, 3]
FOR row = 0 TO 2
  FOR col = 0 TO 2
    LET result[row, col] = row * 3 + col + 1
  NEXT col
NEXT row

' Using DATA/READ with matrices
DATA 1, 2, 3
DATA 4, 5, 6
DIM mat[2, 3]
FOR i = 0 TO 1
  FOR j = 0 TO 2
    READ mat[i, j]
  NEXT j
NEXT i
```

Key points:
- Arrays use row-major order (last index varies fastest)
- Zero-indexed like all PrismBASIC arrays
- Can mix fixed and dynamic dimensions
- Use comma-separated indices: `arr[i, j, k]`
- Query dimensions with ARR.DIMS() and ARR.SIZE()
- Compatible with FOR loops and DATA/READ

## Hash Maps (Associative Arrays)

Store and retrieve data using string keys:

```basic
' Create user profile
CALL Map.Set("user", "name", "Alice")
CALL Map.Set("user", "age", "30")
CALL Map.Set("user", "role", "Engineer")

' Retrieve values
CALL Map.Get("user", "name")
PRINT "Name: "; _

' Check if key exists
CALL Map.Has("user", "email")
IF _ == 0 THEN
  PRINT "No email on file"
ENDIF

' Get with default value
CALL Map.Get("user", "phone", "N/A")
PRINT "Phone: "; _

' Get all keys
CALL Map.Keys("user")
PRINT "Profile fields:"
PRINT _

' Get map size
CALL Map.Size("user")
PRINT "Total fields: "; _

' Delete a key
CALL Map.Delete("user", "age")

' Clear entire map
CALL Map.Clear("user")
```

Common use cases:

```basic
' Configuration management
CALL Map.Set("config", "theme", "dark")
CALL Map.Set("config", "language", "en")

' Word frequency counter
CALL Map.Get("freq", "the", "0")
LET count = _
LET count = count + 1
CALL Map.Set("freq", "the", count)

' Feature flags
CALL Map.Set("features", "beta_mode", "1")
CALL Map.Has("features", "beta_mode")
IF _ == 1 THEN
  PRINT "Beta features enabled"
ENDIF

' Multiple independent maps
CALL Map.Set("session", "token", "abc123")
CALL Map.Set("cache", "last_result", "42")
CALL Map.Set("prefs", "font_size", "14")
```

Key points:
- Keys must be strings
- Values can be strings or numbers
- Multiple independent maps supported
- Case-sensitive keys
- Default values prevent missing-key errors

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

## FOR/NEXT Loops

Traditional BASIC loop structure with optional STEP:

```basic
' Count upward
FOR i = 1 TO 10
  PRINT i
NEXT i

' With step
FOR i = 0 TO 100 STEP 5
  PRINT i
NEXT i

' Countdown (negative step)
FOR i = 10 TO 1 STEP -1
  PRINT "T-minus "; i
NEXT i

' Nested loops
FOR row = 0 TO 2
  FOR col = 0 TO 2
    PRINT "Position ["; row; ", "; col; "]"
  NEXT col
NEXT row

' Use with arrays
DIM squares[10]
FOR i = 0 TO 9
  LET squares[i] = i * i
NEXT i
```

Key points:
- Variable name after NEXT is optional but recommended for clarity
- STEP defaults to 1 if omitted
- Negative STEP allows countdown
- Loop executes if start/end relationship matches step direction
- Nested loops work as expected

## DATA/READ/RESTORE

Embed data directly in your programs:

```basic
' Define data (numbers and strings)
DATA 10, 20, 30
DATA "Alice", "Bob", "Charlie"
DATA 3.14, 2.71, 1.41

' Read sequentially
READ x, y, z
PRINT x, y, z  ' 10, 20, 30

READ name1, name2, name3
PRINT name1, name2, name3

' Reset to beginning
RESTORE
READ first
PRINT first  ' 10 (re-read)

' Read into arrays
DIM values[5]
RESTORE
FOR i = 0 TO 4
  READ values[i]
NEXT i

' Multiple DATA statements accumulate
DATA 100
DATA 200
DATA 300
READ a, b, c  ' Gets 100, 200, 300
```

Key points:
- DATA values accumulate in program order
- READ consumes values sequentially
- RESTORE resets to start of data pool
- Can read into array elements: `READ arr[i]`
- Supports numbers and strings
- Reading past end causes "Out of DATA" error

## String Manipulation

Comprehensive string processing functions:

```basic
' Substring extraction
CALL LEFT("Hello World", 5)
PRINT _  ' "Hello"

CALL RIGHT("Hello World", 5)
PRINT _  ' "World"

CALL MID("Hello World", 7, 5)
PRINT _  ' "World"

' Search for substring
CALL INSTR("Hello World", "World")
PRINT "Position: "; _  ' 7

' Case conversion
CALL UCASE("hello")
PRINT _  ' "HELLO"

CALL LCASE("WORLD")
PRINT _  ' "world"

' Whitespace removal
CALL TRIM("  Hello  ")
PRINT _  ' "Hello"

' Character/ASCII conversion
CALL CHR(65)
PRINT _  ' "A"

CALL ASC("Hello")
PRINT _  ' 72 (ASCII of 'H')

' Number/string conversion
CALL STR(123.45)
LET s = _
PRINT s  ' "123.45"

CALL VAL("42")
PRINT _  ' 42

' Parsing example
LET csv = "Alice,30,Engineer"
CALL INSTR(csv, ",")
LET pos = _
CALL LEFT(csv, pos - 1)
PRINT "Name: "; _  ' "Alice"
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

## Boolean Operators

Combine conditions with AND, OR, NOT:

```basic
LET x = 5
LET y = 10

' AND operator
IF x > 0 AND y > 0 THEN
  PRINT "Both positive"
ENDIF

' OR operator
IF x < 0 OR y > 0 THEN
  PRINT "At least one condition true"
ENDIF

' NOT operator
IF NOT (x == y) THEN
  PRINT "Values are different"
ENDIF

' Combining operators
IF (x > 0 AND x < 10) OR y > 5 THEN
  PRINT "Complex condition"
ENDIF
```

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

' Array introspection
DIM matrix[5, 10]
CALL ARR.DIMS("matrix")
PRINT "Dimensions: "; _
CALL ARR.SIZE("matrix", 0)
PRINT "Rows: "; _
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
' Contact manager with hash maps

10 PRINT "Contact Manager"
20 PRINT "==============="
30 PRINT ""

' Add contacts
40 CALL Map.Set("contacts.alice", "email", "alice@example.com")
50 CALL Map.Set("contacts.alice", "phone", "555-1234")
60 CALL Map.Set("contacts.alice", "role", "Developer")

70 CALL Map.Set("contacts.bob", "email", "bob@example.com")
80 CALL Map.Set("contacts.bob", "phone", "555-5678")
90 CALL Map.Set("contacts.bob", "role", "Designer")

' Display contact info
100 PRINT "Contact: Alice"
110 CALL Map.Get("contacts.alice", "email")
120 PRINT "  Email: "; _
130 CALL Map.Get("contacts.alice", "phone")
140 PRINT "  Phone: "; _
150 CALL Map.Get("contacts.alice", "role")
160 PRINT "  Role: "; _
170 PRINT ""

180 PRINT "Contact: Bob"
190 CALL Map.Get("contacts.bob", "email")
200 PRINT "  Email: "; _
210 CALL Map.Get("contacts.bob", "phone")
220 PRINT "  Phone: "; _
230 CALL Map.Get("contacts.bob", "role")
240 PRINT "  Role: "; _

250 END
```

## Next Steps

- Read the [Language Reference](Language.md) for complete syntax
- Learn about [Mods](Mods.md) for extending the shell
- Customize your [Prompt](Prompt.md)
- Check [Troubleshooting](Troubleshooting.md) if you encounter issues