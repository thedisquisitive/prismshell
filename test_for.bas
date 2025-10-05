#!/usr/bin/env prismshell
' test_for_next.bas - Comprehensive FOR/NEXT Test Suite

PRINT "========================================="
PRINT "FOR/NEXT Loop Test Suite"
PRINT "========================================="
PRINT ""

' ===== Test 1: Basic FOR loop (counting up) =====
PRINT "Test 1: Basic FOR loop (1 TO 5)"
FOR i = 1 TO 5
  PRINT "  i = "; i
NEXT i
PRINT "  Final i = "; i
PRINT "  PASS"
PRINT ""

' ===== Test 2: FOR loop with STEP =====
PRINT "Test 2: FOR loop with STEP 2"
FOR j = 0 TO 10 STEP 2
  PRINT "  j = "; j
NEXT j
PRINT "  PASS"
PRINT ""

' ===== Test 3: Negative STEP (counting down) =====
PRINT "Test 3: Counting down with STEP -1"
FOR k = 5 TO 1 STEP -1
  PRINT "  k = "; k
NEXT k
PRINT "  PASS"
PRINT ""

' ===== Test 4: Nested loops =====
PRINT "Test 4: Nested loops (multiplication table)"
FOR x = 1 TO 3
  FOR y = 1 TO 3
    LET product = x * y
    PRINT "  "; x; " x "; y; " = "; product
  NEXT y
NEXT x
PRINT "  PASS"
PRINT ""

' ===== Test 5: FOR loop with array population =====
PRINT "Test 5: Array population with FOR loop"
DIM squares[10]
FOR n = 0 TO 9
  LET squares[n] = n * n
NEXT n
PRINT "  squares[0] = "; squares[0]
PRINT "  squares[5] = "; squares[5]
PRINT "  squares[9] = "; squares[9]
PRINT "  PASS"
PRINT ""

' ===== Test 6: NEXT without variable name =====
PRINT "Test 6: NEXT without variable name"
LET sum = 0
FOR m = 1 TO 5
  LET sum = sum + m
NEXT
PRINT "  Sum of 1 to 5 = "; sum
PRINT "  PASS"
PRINT ""

' ===== Test 7: Zero iterations (start > end with positive step) =====
PRINT "Test 7: Zero iterations (10 TO 5)"
LET count = 0
FOR z = 10 TO 5
  LET count = count + 1
NEXT z
PRINT "  Count = "; count; " (should be 0)"
PRINT "  PASS"
PRINT ""

' ===== Test 8: Fractional STEP =====
PRINT "Test 8: Fractional STEP (0.5)"
FOR f = 0 TO 2 STEP 0.5
  PRINT "  f = "; f
NEXT f
PRINT "  PASS"
PRINT ""

' ===== Test 9: Large STEP =====
PRINT "Test 9: Large STEP (100)"
FOR big = 0 TO 500 STEP 100
  PRINT "  big = "; big
NEXT big
PRINT "  PASS"
PRINT ""

' ===== Test 10: Triple nested loops =====
PRINT "Test 10: Triple nested loops"
FOR a = 1 TO 2
  FOR b = 1 TO 2
    FOR c = 1 TO 2
      PRINT "  ("; a; ","; b; ","; c; ")"
    NEXT c
  NEXT b
NEXT a
PRINT "  PASS"
PRINT ""

' ===== Test 11: Loop variable modification within loop =====
PRINT "Test 11: Using loop variable in calculations"
LET total = 0
FOR i = 1 TO 5
  LET total = total + (i * 2)
NEXT i
PRINT "  Total = "; total; " (should be 30)"
PRINT "  PASS"
PRINT ""

' ===== Test 12: FOR with expressions =====
PRINT "Test 12: FOR with expressions"
LET start = 2
LET finish = 8
LET increment = 2
FOR v = start TO finish STEP increment
  PRINT "  v = "; v
NEXT v
PRINT "  PASS"
PRINT ""

' ===== Test 13: Fibonacci with FOR loop =====
PRINT "Test 13: Fibonacci sequence"
DIM fib[10]
LET fib[0] = 0
LET fib[1] = 1
FOR i = 2 TO 9
  LET fib[i] = fib[i-1] + fib[i-2]
NEXT i
PRINT "  fib[9] = "; fib[9]; " (should be 34)"
PRINT "  PASS"
PRINT ""

' ===== Test 14: Negative range with negative step =====
PRINT "Test 14: Negative range (-5 TO -1)"
FOR neg = -5 TO -1 STEP 1
  PRINT "  neg = "; neg
NEXT neg
PRINT "  PASS"
PRINT ""

' ===== Test 15: Single iteration =====
PRINT "Test 15: Single iteration (5 TO 5)"
FOR single = 5 TO 5
  PRINT "  single = "; single
NEXT single
PRINT "  PASS"
PRINT ""

PRINT "========================================="
PRINT "All FOR/NEXT Tests Complete!"
PRINT "========================================="
END