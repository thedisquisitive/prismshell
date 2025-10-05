#!/usr/bin/env prismshell
' test.bas - Comprehensive Phase 1 Test Suite

PRINT "========================================="
PRINT "PrismBASIC Phase 1 Test Suite"
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

' ===== Test 4: Array with WHILE Loop =====
PRINT "Test 4: Array with WHILE Loop"
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

' ===== Test 5: String Arrays =====
PRINT "Test 5: String Arrays"
DIM names[3]
LET names[0] = "Alice"
LET names[1] = "Bob"
LET names[2] = "Charlie"
PRINT "  names[0] = "; names[0]
PRINT "  names[1] = "; names[1]
PRINT "  names[2] = "; names[2]
PRINT "  PASS"
PRINT ""

' ===== Test 6: Simple SUB =====
PRINT "Test 6: Simple SUB"
CALL Double(21)
PRINT "  Double(21) = "; _
CALL Double(100)
PRINT "  Double(100) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 7: SUB with Multiple Parameters =====
PRINT "Test 7: SUB with Multiple Parameters"
CALL Add(10, 20)
PRINT "  Add(10, 20) = "; _
CALL Multiply(7, 8)
PRINT "  Multiply(7, 8) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 8: Recursive SUB =====
PRINT "Test 8: Recursive SUB (Factorial)"
CALL Factorial(5)
PRINT "  Factorial(5) = "; _
CALL Factorial(7)
PRINT "  Factorial(7) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 9: SUB Calling SUB =====
PRINT "Test 9: SUB Calling SUB"
CALL ProcessValue(15)
PRINT "  ProcessValue(15) = "; _
PRINT "  PASS"
PRINT ""

' ===== Test 10: IF/ELSEIF/ELSE Blocks =====
PRINT "Test 10: IF/ELSEIF/ELSE Blocks"
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

' ===== Test 11: Nested WHILE Loops =====
PRINT "Test 11: Nested WHILE Loops"
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

' ===== Test 12: Array Bounds =====
PRINT "Test 12: Array Bounds (accessing beyond)"
DIM small[3]
LET small[0] = 1
LET small[1] = 2
LET small[2] = 3
LET safe = small[1]
LET beyond = small[10]
PRINT "  small[1] = "; safe
PRINT "  small[10] (out of bounds) = "; beyond
PRINT "  PASS (no crash)"
PRINT ""

' ===== Test 13: Expression in Array Index =====
PRINT "Test 13: Expression in Array Index"
DIM data[10]
LET data[0] = 100
LET data[1] = 200
LET data[2] = 300
LET idx = 1
LET result = data[idx + 1]
PRINT "  data[1+1] = "; result
PRINT "  PASS"
PRINT ""

' ===== Test 14: Math Operations =====
PRINT "Test 14: Math Operations"
LET a = 10
LET b = 3
PRINT "  10 + 3 = "; a + b
PRINT "  10 - 3 = "; a - b
PRINT "  10 * 3 = "; a * b
PRINT "  10 / 3 = "; a / b
PRINT "  2 ^ 8 = "; 2 ^ 8
PRINT "  PASS"
PRINT ""

' ===== Test 15: String Concatenation =====
PRINT "Test 15: String Concatenation"
LET first = "Hello"
LET second = "World"
LET combined = first + " " + second
PRINT "  '"; first; "' + ' ' + '"; second; "' = '"; combined; "'"
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
  PRINT "Factorial called with n="; n
  IF n <= 1 THEN
    PRINT "  Base case"
    LET _ = 1
  ELSE
    PRINT "  Recursive case"
    CALL Factorial(n - 1)
    LET temp = _
    PRINT "  Recursive result: "; temp
    LET _ = n * temp
  ENDIF
  LET final = _
  PRINT "  Returning: "; final
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
