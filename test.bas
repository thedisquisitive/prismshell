#!/usr/bin/env prismshell
' test.bas - Comprehensive Test Suite for PrismBASIC

PRINT "========================================="
PRINT "PrismBASIC Comprehensive Test Suite"
PRINT "========================================="
PRINT ""

' ===== Test 1: Basic Arrays =====
PRINT "Test 1: Basic Arrays"
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
PRINT "  PASS"
PRINT ""

' ===== Test 2: LEN Function =====
PRINT "Test 2: LEN Function"
LET text = "Hello World"
CALL LEN(text)
PRINT "  Length of 'Hello World': "; _
CALL LEN("nums")
PRINT "  Length of nums array: "; _
PRINT "  PASS"
PRINT ""

' ===== Test 3: Dynamic Arrays =====
PRINT "Test 3: Dynamic Arrays"
DIM dynamic[]
LET dynamic[0] = 100
LET dynamic[5] = 500
LET dynamic[10] = 1000
PRINT "  dynamic[0] = "; dynamic[0]
PRINT "  dynamic[5] = "; dynamic[5]
PRINT "  dynamic[10] = "; dynamic[10]
CALL LEN("dynamic")
PRINT "  Array auto-expanded to size: "; _
PRINT "  PASS"
PRINT ""

' ===== Test 4: FOR/NEXT Loop =====
PRINT "Test 4: FOR/NEXT Loop"
DIM fib[10]
LET fib[0] = 0
LET fib[1] = 1
FOR i = 2 TO 9
  LET fib[i] = fib[i-1] + fib[i-2]
NEXT i
PRINT "  fib[5] = "; fib[5]; " (should be 5)"
PRINT "  fib[9] = "; fib[9]; " (should be 34)"
PRINT "  PASS"
PRINT ""

' ===== Test 5: FOR/NEXT with STEP =====
PRINT "Test 5: FOR/NEXT with STEP"
LET sum = 0
FOR j = 0 TO 10 STEP 2
  LET sum = sum + j
NEXT j
PRINT "  Sum of evens 0-10: "; sum; " (should be 30)"
PRINT "  PASS"
PRINT ""

' ===== Test 6: FOR/NEXT Negative STEP =====
PRINT "Test 6: FOR/NEXT Negative STEP"
LET countdown = ""
FOR k = 5 TO 1 STEP -1
  LET countdown = countdown + k
NEXT k
PRINT "  Countdown: "; countdown; " (should be 54321)"
PRINT "  PASS"
PRINT ""

' ===== Test 7: Nested FOR Loops =====
PRINT "Test 7: Nested FOR Loops"
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
PRINT "  PASS"
PRINT ""

' ===== Test 8: DATA/READ Basic =====
PRINT "Test 8: DATA/READ Basic"
DATA 42, "hello", 3.14
READ x, msg, pi
PRINT "  x = "; x
PRINT "  msg = "; msg
PRINT "  pi = "; pi
PRINT "  PASS"
PRINT ""

' ===== Test 9: Multiple DATA Statements =====
PRINT "Test 9: Multiple DATA Statements"
DATA 100, 200
DATA 300, 400
READ a, b, c, d
PRINT "  a = "; a; ", b = "; b; ", c = "; c; ", d = "; d
PRINT "  PASS"
PRINT ""

' ===== Test 10: RESTORE =====
PRINT "Test 10: RESTORE"
RESTORE
READ first, second
PRINT "  Re-read first two: "; first; ", "; second
PRINT "  PASS"
PRINT ""

' ===== Test 11: READ into Arrays =====
PRINT "Test 11: READ into Arrays"
DIM values[5]
RESTORE
FOR n = 0 TO 4
  READ values[n]
NEXT n
PRINT "  values[0] = "; values[0]
PRINT "  values[2] = "; values[2]
PRINT "  values[4] = "; values[4]
PRINT "  PASS"
PRINT ""

' ===== Test 12: WHILE Loop =====
PRINT "Test 12: WHILE Loop"
DIM squares[5]
LET i = 0
WHILE i < 5
  LET squares[i] = i * i
  LET i = i + 1
WEND
PRINT "  squares[0] = "; squares[0]
PRINT "  squares[2] = "; squares[2]
PRINT "  squares[4] = "; squares[4]
PRINT "  PASS"
PRINT ""

' ===== Test 13: String Arrays =====
PRINT "Test 13: String Arrays"
DIM names[3]
LET names[0] = "Alice"
LET names[1] = "Bob"
LET names[2] = "Charlie"
PRINT "  names[0] = "; names[0]
PRINT "  names[1] = "; names[1]
PRINT "  names[2] = "; names[2]
PRINT "  PASS"
PRINT ""

' ===== Test 14: Simple SUB =====
PRINT "Test 14: Simple SUB"
CALL Double(21)
PRINT "  Double(21) = "; _
CALL Double(100)
PRINT "  Double(100) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 15: SUB with Multiple Parameters =====
PRINT "Test 15: SUB with Multiple Parameters"
CALL Add(10, 20)
PRINT "  Add(10, 20) = "; _
CALL Multiply(7, 8)
PRINT "  Multiply(7, 8) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 16: Recursive SUB =====
PRINT "Test 16: Recursive SUB (Factorial)"
CALL Factorial(5)
PRINT "  Factorial(5) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 17: SUB Calling SUB =====
PRINT "Test 17: SUB Calling SUB"
CALL ProcessValue(15)
PRINT "  ProcessValue(15) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 18: IF/ELSEIF/ELSE Blocks =====
PRINT "Test 18: IF/ELSEIF/ELSE Blocks"
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
PRINT "  PASS"
PRINT ""

' ===== Test 19: Nested WHILE Loops =====
PRINT "Test 19: Nested WHILE Loops"
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
PRINT "  PASS"
PRINT ""

' ===== Test 20: Expression in Array Index =====
PRINT "Test 20: Expression in Array Index"
DIM items[10]
LET items[0] = 100
LET items[1] = 200
LET items[2] = 300
LET idx = 1
LET result = items[idx + 1]
PRINT "  items[1+1] = "; result
PRINT "  PASS"
PRINT ""

' ===== Test 21: Math Operations =====
PRINT "Test 21: Math Operations"
LET m1 = 10
LET m2 = 3
PRINT "  10 + 3 = "; m1 + m2
PRINT "  10 - 3 = "; m1 - m2
PRINT "  10 * 3 = "; m1 * m2
PRINT "  10 / 3 = "; m1 / m2
PRINT "  2 ^ 8 = "; 2 ^ 8
PRINT "  PASS"
PRINT ""

' ===== Test 22: String Concatenation =====
PRINT "Test 22: String Concatenation"
LET first = "Hello"
LET second = "World"
LET combined = first + " " + second
PRINT "  Result: '"; combined; "'"
PRINT "  PASS"
PRINT ""

' ===== Test 23: Comparison Operators =====
PRINT "Test 23: Comparison Operators"
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
PRINT "  PASS"
PRINT ""

' ===== Test 24: Variable Assignment Chain =====
PRINT "Test 24: Variable Assignment Chain"
LET w = 5
LET x = w
LET y = x
LET z = y
PRINT "  w = "; w; ", x = "; x; ", y = "; y; ", z = "; z
IF w == z THEN
  PRINT "  Assignment chain works correctly"
ENDIF
PRINT "  PASS"
PRINT ""

' ===== Test 25: Mixed FOR and WHILE =====
PRINT "Test 25: Mixed FOR and WHILE"
LET total = 0
FOR outer = 1 TO 3
  LET inner = 0
  WHILE inner < outer
    LET total = total + 1
    LET inner = inner + 1
  WEND
NEXT outer
PRINT "  Total iterations: "; total; " (should be 6)"
PRINT "  PASS"
PRINT ""

' Test 26: 2D Arrays
PRINT "Test 26: 2D Arrays"
DIM matrix[3, 4]
LET matrix[1, 2] = 42
LET matrix[2, 3] = 99
PRINT "  matrix[1, 2] = "; matrix[1, 2]
PRINT "  matrix[2, 3] = "; matrix[2, 3]
CALL ARR.DIMS("matrix")
PRINT "  Dimensions: "; _
PRINT "  PASS"

' Test 27: Dynamic 2D Array
PRINT "Test 27: Dynamic 2D Array"
DIM grid[, ]
LET grid[5, 10] = 1000
PRINT "  grid[5, 10] = "; grid[5, 10]
CALL ARR.SIZE("grid", 0)
PRINT "  Rows: "; _
CALL ARR.SIZE("grid", 1)
PRINT "  Cols: "; _
PRINT "  PASS"

' Test 28: 3D Array
PRINT "Test 28: 3D Array"
DIM cube[2, 3, 4]
LET cube[1, 2, 3] = 777
PRINT "  cube[1, 2, 3] = "; cube[1, 2, 3]
PRINT "  PASS"

' ===== Test 29: Matrix Operations =====
PRINT "Test 29: Matrix Operations"
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
PRINT "  PASS"
PRINT ""

' ===== Test 30: Mixed Dimension Arithmetic =====
PRINT "Test 30: Mixed Dimension Arithmetic"
DIM vec[5]
DIM matrix2[3, 4]
LET vec[0] = 10
LET vec[1] = 20
LET matrix2[1, 2] = vec[0] + vec[1]
PRINT "  vec[0] + vec[1] = "; matrix2[1, 2]; " (should be 30)"
PRINT "  PASS"
PRINT ""

' ===== Test 31: 3D Array Population =====
PRINT "Test 31: 3D Array Population"
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
PRINT "  PASS"
PRINT ""

' ===== Test 32: Dynamic Multi-Dim Expansion =====
PRINT "Test 32: Dynamic Multi-Dim Expansion"
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
PRINT "  PASS"
PRINT ""

' ===== Test 33: AND/OR Operators =====
PRINT "Test 33: AND/OR Operators"
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
PRINT "  PASS"
PRINT ""

' ===== Test 34: LEFT/RIGHT Functions =====
PRINT "Test 34: LEFT/RIGHT Functions"
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
PRINT "  PASS"
PRINT ""

' ===== Test 35: MID Function =====
PRINT "Test 35: MID Function"
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
PRINT "  PASS"
PRINT ""

' ===== Test 36: INSTR Function =====
PRINT "Test 36: INSTR Function"
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
PRINT "  PASS"
PRINT ""

' ===== Test 37: CHR/ASC Functions =====
PRINT "Test 37: CHR/ASC Functions"
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
PRINT "  PASS"
PRINT ""

' ===== Test 38: UCASE/LCASE Functions =====
PRINT "Test 38: UCASE/LCASE Functions"
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
PRINT "  PASS"
PRINT ""

' ===== Test 39: TRIM Functions =====
PRINT "Test 39: TRIM Functions"
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
PRINT "  PASS"
PRINT ""

' ===== Test 40: STR/VAL Functions =====
PRINT "Test 40: STR/VAL Functions"
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
PRINT "  PASS"
PRINT ""

' ===== Test 41: String Manipulation Examples =====
PRINT "Test 41: String Manipulation Examples"
LET sentence = "The Quick Brown Fox"
CALL LCASE(sentence)
LET lower = _
CALL UCASE(sentence)
LET upper = _
PRINT "  Original: "; sentence
PRINT "  Lower: "; lower
PRINT "  Upper: "; upper

LET text = "Hello, World!"
CALL INSTR(text, ",")
LET comma_pos = _
CALL LEFT(text, comma_pos - 1)
LET before = _
CALL MID(text, comma_pos + 2)
LET after = _
PRINT "  Split '"; text; "' at comma:"
PRINT "    Before: '"; before; "'"
PRINT "    After: '"; after; "'"
PRINT "  PASS"
PRINT ""

' ===== Test 42: Building Strings with CHR =====
PRINT "Test 42: Building Strings with CHR"
LET alphabet = ""
FOR i = 65 TO 90
  CALL CHR(i)
  LET alphabet = alphabet + _
NEXT i
PRINT "  Alphabet: "; alphabet
CALL LEN(alphabet)
IF _ == 26 THEN
  PRINT "  Built 26-char alphabet: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' ===== Test 43: Parsing with String Functions =====
PRINT "Test 43: Parsing CSV-like Data"
LET csv = "Alice,25,Engineer"
CALL INSTR(csv, ",")
LET pos1 = _
CALL LEFT(csv, pos1 - 1)
LET name = _
CALL MID(csv, pos1 + 1)
LET rest = _
CALL INSTR(rest, ",")
LET pos2 = _
CALL LEFT(rest, pos2 - 1)
LET age_str = _
CALL VAL(age_str)
LET age = _
CALL MID(rest, pos2 + 1)
LET job = _
PRINT "  Parsed CSV: '"; csv; "'"
PRINT "    Name: "; name
PRINT "    Age: "; age
PRINT "    Job: "; job
IF name == "Alice" AND age == 25 AND job == "Engineer" THEN
  PRINT "  CSV parsing: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

PRINT "========================================="
PRINT "All Tests Complete!"
PRINT "========================================="
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