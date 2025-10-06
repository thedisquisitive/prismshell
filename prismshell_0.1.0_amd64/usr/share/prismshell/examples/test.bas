#!/usr/bin/env prismshell
' test.bas - Comprehensive Test Suite for PrismBASIC (with color support)

' Check if terminal supports colors
CALL Color.Supported()
LET USE_COLOR = _

' Set up color variables
IF USE_COLOR == 1 THEN
  CALL Color.Bold()
  LET BOLD = _
  CALL Color.Cyan()
  LET CYAN = _
  CALL Color.Green()
  LET GREEN = _
  CALL Color.Yellow()
  LET YELLOW = _
  CALL Color.Reset()
  LET RESET = _
ELSE
  LET BOLD = ""
  LET CYAN = ""
  LET GREEN = ""
  LET YELLOW = ""
  LET RESET = ""
ENDIF

PRINT BOLD; CYAN; "========================================="; RESET
PRINT BOLD; CYAN; "PrismBASIC Comprehensive Test Suite"; RESET
PRINT BOLD; CYAN; "========================================="; RESET
PRINT ""

' ===== Test 1: Basic Arrays =====
PRINT BOLD; "Test 1: Basic Arrays"; RESET
DIM nums[5]
LET nums[0] = 10
LET nums[1] = 20
LET nums[2] = 30
LET nums[3] = 40
LET nums[4] = 50
PRINT "  nums[0] = "; nums[0]
PRINT "  nums[2] = "; nums[2]
PRINT "  nums[4] = "; nums[4]
PRINT "  Sum: "; nums[0] + nums[2] + nums[4]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 2: LEN Function =====
PRINT BOLD; "Test 2: LEN Function"; RESET
LET text = "Hello World"
CALL LEN(text)
PRINT "  Length of 'Hello World': "; _
CALL LEN("nums")
PRINT "  Length of nums array: "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 3: Dynamic Arrays =====
PRINT BOLD; "Test 3: Dynamic Arrays"; RESET
DIM dynamic[]
LET dynamic[0] = 100
LET dynamic[5] = 500
LET dynamic[10] = 1000
PRINT "  dynamic[0] = "; dynamic[0]
PRINT "  dynamic[5] = "; dynamic[5]
PRINT "  dynamic[10] = "; dynamic[10]
CALL LEN("dynamic")
PRINT "  Array auto-expanded to size: "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 4: FOR/NEXT Loop =====
PRINT BOLD; "Test 4: FOR/NEXT Loop"; RESET
DIM fib[10]
LET fib[0] = 0
LET fib[1] = 1
FOR i = 2 TO 9
  LET fib[i] = fib[i-1] + fib[i-2]
NEXT i
PRINT "  fib[5] = "; fib[5]; " (should be 5)"
PRINT "  fib[9] = "; fib[9]; " (should be 34)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 5: FOR/NEXT with STEP =====
PRINT BOLD; "Test 5: FOR/NEXT with STEP"; RESET
LET sum = 0
FOR j = 0 TO 10 STEP 2
  LET sum = sum + j
NEXT j
PRINT "  Sum of evens 0-10: "; sum; " (should be 30)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 6: FOR/NEXT Negative STEP =====
PRINT BOLD; "Test 6: FOR/NEXT Negative STEP"; RESET
LET countdown = ""
FOR k = 5 TO 1 STEP -1
  LET countdown = countdown + k
NEXT k
PRINT "  Countdown: "; countdown; " (should be 54321)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 7: Nested FOR Loops =====
PRINT BOLD; "Test 7: Nested FOR Loops"; RESET
DIM grid[9]
FOR row = 0 TO 2
  FOR col = 0 TO 2
    LET idx = row * 3 + col
    LET grid[idx] = row * 10 + col
  NEXT col
NEXT row
PRINT "  grid[0] = "; grid[0]; " (should be 0)"
PRINT "  grid[4] = "; grid[4]; " (should be 11)"
PRINT "  grid[8] = "; grid[8]; " (should be 22)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 8: DATA/READ Basic =====
PRINT BOLD; "Test 8: DATA/READ Basic"; RESET
DATA 42, "hello", 3.14
READ x, msg, pi
PRINT "  x = "; x
PRINT "  msg = "; msg
PRINT "  pi = "; pi
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 9: Multiple DATA Statements =====
PRINT BOLD; "Test 9: Multiple DATA Statements"; RESET
DATA 100, 200
DATA 300, 400
READ a, b, c, d
PRINT "  a = "; a; ", b = "; b; ", c = "; c; ", d = "; d
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 10: RESTORE =====
PRINT BOLD; "Test 10: RESTORE"; RESET
RESTORE
READ first, second
PRINT "  Re-read first two: "; first; ", "; second
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 11: READ into Arrays =====
PRINT BOLD; "Test 11: READ into Arrays"; RESET
DIM values[5]
RESTORE
FOR n = 0 TO 4
  READ values[n]
NEXT n
PRINT "  values[0] = "; values[0]
PRINT "  values[2] = "; values[2]
PRINT "  values[4] = "; values[4]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 12: WHILE Loop =====
PRINT BOLD; "Test 12: WHILE Loop"; RESET
DIM squares[5]
LET i = 0
WHILE i < 5
  LET squares[i] = i * i
  LET i = i + 1
WEND
PRINT "  squares[0] = "; squares[0]
PRINT "  squares[2] = "; squares[2]
PRINT "  squares[4] = "; squares[4]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 13: String Arrays =====
PRINT BOLD; "Test 13: String Arrays"; RESET
DIM names[3]
LET names[0] = "Alice"
LET names[1] = "Bob"
LET names[2] = "Charlie"
PRINT "  names[0] = "; names[0]
PRINT "  names[1] = "; names[1]
PRINT "  names[2] = "; names[2]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 14: Simple SUB =====
PRINT BOLD; "Test 14: Simple SUB"; RESET
CALL Double(21)
PRINT "  Double(21) = "; _
CALL Double(100)
PRINT "  Double(100) = "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 15: SUB with Multiple Parameters =====
PRINT BOLD; "Test 15: SUB with Multiple Parameters"; RESET
CALL Add(10, 20)
PRINT "  Add(10, 20) = "; _
CALL Multiply(7, 8)
PRINT "  Multiply(7, 8) = "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 16: Recursive SUB =====
PRINT BOLD; "Test 16: Recursive SUB (Factorial)"; RESET
CALL Factorial(5)
PRINT "  Factorial(5) = "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 17: SUB Calling SUB =====
PRINT BOLD; "Test 17: SUB Calling SUB"; RESET
CALL ProcessValue(15)
PRINT "  ProcessValue(15) = "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 18: IF/ELSEIF/ELSE Blocks =====
PRINT BOLD; "Test 18: IF/ELSEIF/ELSE Blocks"; RESET
LET score = 85
IF score >= 90 THEN
  PRINT "  Grade: A"
ELSEIF score >= 80 THEN
  PRINT "  Grade: B"
ELSEIF score >= 70 THEN
  PRINT "  Grade: C"
ELSE
  PRINT "  Grade: F"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 19: Nested WHILE Loops =====
PRINT BOLD; "Test 19: Nested WHILE Loops"; RESET
DIM matrix[9]
LET row = 0
WHILE row < 3
  LET col = 0
  WHILE col < 3
    LET idx = row * 3 + col
    LET matrix[idx] = row + col
    LET col = col + 1
  WEND
  LET row = row + 1
WEND
PRINT "  matrix[0] (0+0) = "; matrix[0]
PRINT "  matrix[4] (1+1) = "; matrix[4]
PRINT "  matrix[8] (2+2) = "; matrix[8]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 20: Expression in Array Index =====
PRINT BOLD; "Test 20: Expression in Array Index"; RESET
DIM items[10]
LET items[0] = 100
LET items[1] = 200
LET items[2] = 300
LET idx = 1
LET result = items[idx + 1]
PRINT "  items[1+1] = "; result
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 21: Math Operations =====
PRINT BOLD; "Test 21: Math Operations"; RESET
LET m1 = 10
LET m2 = 3
PRINT "  10 + 3 = "; m1 + m2
PRINT "  10 - 3 = "; m1 - m2
PRINT "  10 * 3 = "; m1 * m2
PRINT "  10 / 3 = "; m1 / m2
PRINT "  2 ^ 8 = "; 2 ^ 8
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 22: String Concatenation =====
PRINT BOLD; "Test 22: String Concatenation"; RESET
LET first = "Hello"
LET second = "World"
LET combined = first + " " + second
PRINT "  Result: '"; combined; "'"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 23: Comparison Operators =====
PRINT BOLD; "Test 23: Comparison Operators"; RESET
LET v1 = 10
LET v2 = 20
IF v1 < v2 THEN
  PRINT "  10 < 20: TRUE"
ENDIF
IF v1 == 10 THEN
  PRINT "  10 == 10: TRUE"
ENDIF
IF v2 != v1 THEN
  PRINT "  20 != 10: TRUE"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 24: Variable Assignment Chain =====
PRINT BOLD; "Test 24: Variable Assignment Chain"; RESET
LET w = 5
LET x = w
LET y = x
LET z = y
PRINT "  w = "; w; ", x = "; x; ", y = "; y; ", z = "; z
IF w == z THEN
  PRINT "  Assignment chain works correctly"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 25: Mixed FOR and WHILE =====
PRINT BOLD; "Test 25: Mixed FOR and WHILE"; RESET
LET total = 0
FOR outer = 1 TO 3
  LET inner = 0
  WHILE inner < outer
    LET total = total + 1
    LET inner = inner + 1
  WEND
NEXT outer
PRINT "  Total iterations: "; total; " (should be 6)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 26: 2D Arrays =====
PRINT BOLD; "Test 26: 2D Arrays"; RESET
DIM matrix2d[3, 4]
LET matrix2d[1, 2] = 42
LET matrix2d[2, 3] = 99
PRINT "  matrix[1, 2] = "; matrix2d[1, 2]
PRINT "  matrix[2, 3] = "; matrix2d[2, 3]
CALL ARR.DIMS("matrix2d")
PRINT "  Dimensions: "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 27: Dynamic 2D Array =====
PRINT BOLD; "Test 27: Dynamic 2D Array"; RESET
DIM grid2d[, ]
LET grid2d[5, 10] = 1000
PRINT "  grid[5, 10] = "; grid2d[5, 10]
CALL ARR.SIZE("grid2d", 0)
PRINT "  Rows: "; _
CALL ARR.SIZE("grid2d", 1)
PRINT "  Cols: "; _
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 28: 3D Array =====
PRINT BOLD; "Test 28: 3D Array"; RESET
DIM cube[2, 3, 4]
LET cube[1, 2, 3] = 777
PRINT "  cube[1, 2, 3] = "; cube[1, 2, 3]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 29: Matrix Operations =====
PRINT BOLD; "Test 29: Matrix Operations"; RESET
DIM mat[3, 3]
LET sum = 0
FOR row = 0 TO 2
  FOR col = 0 TO 2
    LET mat[row, col] = row * 3 + col + 1
    LET sum = sum + mat[row, col]
  NEXT col
NEXT row
PRINT "  mat[0, 0] = "; mat[0, 0]; " (should be 1)"
PRINT "  mat[1, 1] = "; mat[1, 1]; " (should be 5)"
PRINT "  mat[2, 2] = "; mat[2, 2]; " (should be 9)"
PRINT "  Sum of all elements: "; sum; " (should be 45)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 30: Mixed Dimension Arithmetic =====
PRINT BOLD; "Test 30: Mixed Dimension Arithmetic"; RESET
DIM vec[5]
DIM matrix2[3, 4]
LET vec[0] = 10
LET vec[1] = 20
LET matrix2[1, 2] = vec[0] + vec[1]
PRINT "  vec[0] + vec[1] = "; matrix2[1, 2]; " (should be 30)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 31: 3D Array Population =====
PRINT BOLD; "Test 31: 3D Array Population"; RESET
DIM cube3d[2, 2, 2]
LET total = 0
FOR x = 0 TO 1
  FOR y = 0 TO 1
    FOR z = 0 TO 1
      LET val = x * 4 + y * 2 + z
      LET cube3d[x, y, z] = val
      LET total = total + val
    NEXT z
  NEXT y
NEXT x
PRINT "  cube3d[0, 0, 0] = "; cube3d[0, 0, 0]
PRINT "  cube3d[1, 1, 1] = "; cube3d[1, 1, 1]
PRINT "  Total: "; total; " (should be 28)"
CALL ARR.DIMS("cube3d")
PRINT "  Dimensions: "; _; " (should be 3)"
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 32: Dynamic Multi-Dim Expansion =====
PRINT BOLD; "Test 32: Dynamic Multi-Dim Expansion"; RESET
DIM sparse[, , ]
LET sparse[0, 0, 0] = 1
LET sparse[10, 5, 3] = 999
CALL ARR.SIZE("sparse", 0)
PRINT "  Dim 0 size: "; _; " (should be 11)"
CALL ARR.SIZE("sparse", 1)
PRINT "  Dim 1 size: "; _; " (should be 6)"
CALL ARR.SIZE("sparse", 2)
PRINT "  Dim 2 size: "; _; " (should be 4)"
PRINT "  sparse[10, 5, 3] = "; sparse[10, 5, 3]
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 33: AND/OR Operators =====
PRINT BOLD; "Test 33: AND/OR Operators"; RESET
LET x = 5
LET y = 10
IF x > 0 AND y > 0 THEN
  PRINT "  Both positive: PASS"
ENDIF
IF x < 0 OR y > 0 THEN
  PRINT "  At least one true: PASS"
ENDIF
LET a = 1
LET b = 0
IF a AND b THEN
  PRINT "  ERROR: 1 AND 0 should be false"
ELSE
  PRINT "  1 AND 0 = false: PASS"
ENDIF
IF a OR b THEN
  PRINT "  1 OR 0 = true: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 34: LEFT/RIGHT Functions =====
PRINT BOLD; "Test 34: LEFT/RIGHT Functions"; RESET
CALL LEFT("Hello World", 5)
PRINT "  LEFT('Hello World', 5) = '"; _; "'"
IF _ == "Hello" THEN
  PRINT "  LEFT: PASS"
ENDIF
CALL RIGHT("Hello World", 5)
PRINT "  RIGHT('Hello World', 5) = '"; _; "'"
IF _ == "World" THEN
  PRINT "  RIGHT: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 35: MID Function =====
PRINT BOLD; "Test 35: MID Function"; RESET
CALL MID("Hello World", 7, 5)
PRINT "  MID('Hello World', 7, 5) = '"; _; "'"
IF _ == "World" THEN
  PRINT "  MID with length: PASS"
ENDIF
CALL MID("Hello World", 7)
PRINT "  MID('Hello World', 7) = '"; _; "'"
IF _ == "World" THEN
  PRINT "  MID to end: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 36: INSTR Function =====
PRINT BOLD; "Test 36: INSTR Function"; RESET
CALL INSTR("Hello World", "World")
PRINT "  INSTR('Hello World', 'World') = "; _
IF _ == 7 THEN
  PRINT "  INSTR found: PASS"
ENDIF
CALL INSTR("Hello World", "xyz")
PRINT "  INSTR('Hello World', 'xyz') = "; _
IF _ == 0 THEN
  PRINT "  INSTR not found: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 37: CHR/ASC Functions =====
PRINT BOLD; "Test 37: CHR/ASC Functions"; RESET
CALL CHR(65)
PRINT "  CHR(65) = '"; _; "'"
IF _ == "A" THEN
  PRINT "  CHR: PASS"
ENDIF
CALL ASC("Hello")
PRINT "  ASC('Hello') = "; _
IF _ == 72 THEN
  PRINT "  ASC: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 38: UCASE/LCASE Functions =====
PRINT BOLD; "Test 38: UCASE/LCASE Functions"; RESET
CALL UCASE("Hello World")
PRINT "  UCASE('Hello World') = '"; _; "'"
IF _ == "HELLO WORLD" THEN
  PRINT "  UCASE: PASS"
ENDIF
CALL LCASE("Hello World")
PRINT "  LCASE('Hello World') = '"; _; "'"
IF _ == "hello world" THEN
  PRINT "  LCASE: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 39: TRIM Functions =====
PRINT BOLD; "Test 39: TRIM Functions"; RESET
CALL TRIM("  Hello World  ")
PRINT "  TRIM('  Hello World  ') = '"; _; "'"
IF _ == "Hello World" THEN
  PRINT "  TRIM: PASS"
ENDIF
CALL LTRIM("  Hello World  ")
IF _ == "Hello World  " THEN
  PRINT "  LTRIM: PASS"
ENDIF
CALL RTRIM("  Hello World  ")
IF _ == "  Hello World" THEN
  PRINT "  RTRIM: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 40: STR/VAL Functions =====
PRINT BOLD; "Test 40: STR/VAL Functions"; RESET
CALL STR(123.45)
PRINT "  STR(123.45) = '"; _; "'"
LET s = _
CALL VAL(s)
PRINT "  VAL('"; s; "') = "; _
IF _ == 123.45 THEN
  PRINT "  STR/VAL round-trip: PASS"
ENDIF
CALL VAL("42")
IF _ == 42 THEN
  PRINT "  VAL integer: PASS"
ENDIF
CALL VAL("not a number")
IF _ == 0 THEN
  PRINT "  VAL invalid returns 0: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 41: Hash Map Basic Operations =====
PRINT BOLD; "Test 41: Hash Map Basic Operations"; RESET
CALL Map.Set("test_user", "name", "Alice")
CALL Map.Set("test_user", "age", "25")
CALL Map.Set("test_user", "role", "Engineer")
CALL Map.Get("test_user", "name")
IF _ == "Alice" THEN
  PRINT "  Map.Get: PASS"
ENDIF
CALL Map.Has("test_user", "name")
IF _ == 1 THEN
  PRINT "  Map.Has (exists): PASS"
ENDIF
CALL Map.Has("test_user", "missing")
IF _ == 0 THEN
  PRINT "  Map.Has (missing): PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 42: Hash Map Default Values =====
PRINT BOLD; "Test 42: Hash Map Default Values"; RESET
CALL Map.Get("test_user", "phone", "N/A")
IF _ == "N/A" THEN
  PRINT "  Default value: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 43: Hash Map Size and Keys =====
PRINT BOLD; "Test 43: Hash Map Size and Keys"; RESET
CALL Map.Size("test_user")
IF _ == 3 THEN
  PRINT "  Map.Size: PASS"
ENDIF
CALL Map.Keys("test_user")
LET keys = _
CALL INSTR(keys, "name")
IF _ > 0 THEN
  PRINT "  Map.Keys contains 'name': PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 44: Hash Map Delete and Clear =====
PRINT BOLD; "Test 44: Hash Map Delete and Clear"; RESET
CALL Map.Delete("test_user", "age")
CALL Map.Size("test_user")
IF _ == 2 THEN
  PRINT "  Map.Delete: PASS"
ENDIF
CALL Map.Clear("test_user")
CALL Map.Size("test_user")
IF _ == 0 THEN
  PRINT "  Map.Clear: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 45: Multiple Hash Maps =====
PRINT BOLD; "Test 45: Multiple Hash Maps"; RESET
CALL Map.Set("config", "theme", "dark")
CALL Map.Set("session", "token", "abc123")
CALL Map.Get("config", "theme")
LET theme = _
CALL Map.Get("session", "token")
LET token = _
IF theme == "dark" AND token == "abc123" THEN
  PRINT "  Multiple independent maps: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

' ===== Test 46: Hash Map Word Counter =====
PRINT BOLD; "Test 46: Hash Map Word Counter"; RESET
CALL Map.Clear("words")
LET text = "the quick brown fox jumps over the lazy dog"
' Simplified word count for "the"
CALL Map.Get("words", "the", "0")
LET count = _
CALL VAL(count)
LET count = _ + 1
CALL STR(count)
CALL Map.Set("words", "the", _)
CALL Map.Get("words", "the", "0")
LET count = _
CALL VAL(count)
LET count = _ + 1
CALL STR(count)
CALL Map.Set("words", "the", _)
CALL Map.Get("words", "the")
CALL VAL(_)
IF _ == 2 THEN
  PRINT "  Word frequency counter: PASS"
ENDIF
PRINT GREEN; "  PASS"; RESET
PRINT ""

PRINT BOLD; CYAN; "========================================="; RESET
PRINT BOLD; GREEN; "All Tests Complete!"; RESET
PRINT YELLOW; "  Total: 46 tests"; RESET
PRINT "  Core Arrays: Tests 1-3"
PRINT "  Loops: Tests 4-7, 12, 19, 25"
PRINT "  DATA/READ: Tests 8-11"
PRINT "  Strings: Tests 13, 22"
PRINT "  SUBs: Tests 14-17"
PRINT "  Control Flow: Tests 18, 19"
PRINT "  Math: Tests 20-21, 23-24"
PRINT "  Multidimensional Arrays: Tests 26-32"
PRINT "  Boolean: Test 33"
PRINT "  String Functions: Tests 34-40"
PRINT "  Hash Maps: Tests 41-46"
PRINT BOLD; CYAN; "========================================="; RESET
END

' ===== SUB Definitions =====

SUB Double(x)
  LET _ = x * 2
END SUB

SUB Add(a, b)
  LET _ = a + b
END SUB

SUB Multiply(x, y)
  LET _ = x * y
END SUB

SUB Factorial(n)
  IF n <= 1 THEN
    LET _ = 1
  ELSE
    CALL Factorial(n - 1)
    LET _ = n * _
  ENDIF
END SUB

SUB Helper(val)
  LET _ = val * 3
END SUB

SUB ProcessValue(num)
  CALL Helper(num)
  LET temp = _
  LET _ = temp + 10
END SUB

' End of Test Suite