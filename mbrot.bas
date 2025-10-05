#!/usr/bin/env prismshell
' ASCII Mandelbrot Set - Debug Version

DIM mandel[40, 80]

' Test a few specific points first
PRINT "Testing specific coordinates:"
LET x0 = 0
LET y0 = 0
LET x = 0
LET y = 0
LET iter = 0

PRINT "Testing (0,0) - should be in set (iter=100):"
WHILE iter < 100 AND (x*x + y*y) < 4
  LET xtemp = x*x - y*y + x0
  LET ytemp = 2*x*y + y0
  LET x = xtemp
  LET y = ytemp
  LET iter = iter + 1
WEND
PRINT "  Iterations: "; iter

' Test a point outside the set
LET x0 = 2
LET y0 = 2
LET x = 0
LET y = 0
LET iter = 0

PRINT "Testing (2,2) - should escape quickly:"
WHILE iter < 100 AND (x*x + y*y) < 4
  LET xtemp = x*x - y*y + x0
  LET ytemp = 2*x*y + y0
  LET x = xtemp
  LET y = ytemp
  LET iter = iter + 1
  IF iter < 10 THEN
    PRINT "  iter "; iter; ": x="; x; " y="; y; " mag="; (x*x + y*y)
  ENDIF
WEND
PRINT "  Final iterations: "; iter
PRINT ""

' Now generate the full set
FOR py = 0 TO 39
  FOR px = 0 TO 79
    LET x0 = (px / 80) * 3.0 - 2.0
    LET y0 = (py / 40) * 2.0 - 1.0
    
    LET x = 0
    LET y = 0
    LET iter = 0
    
    WHILE iter < 100 AND (x*x + y*y) < 4
      LET xtemp = x*x - y*y + x0
      LET ytemp = 2*x*y + y0
      LET x = xtemp
      LET y = ytemp
      LET iter = iter + 1
    WEND
    
    LET mandel[py, px] = iter
  NEXT px
NEXT py

' Render
FOR py = 0 TO 39
  FOR px = 0 TO 79
    LET val = mandel[py, px]
    IF val >= 100 THEN
      PRINT " ";
    ELSEIF val >= 20 THEN
      PRINT ".";
    ELSEIF val >= 10 THEN
      PRINT "+";
    ELSEIF val >= 5 THEN
      PRINT "*";
    ELSE
      PRINT "#";
    ENDIF
  NEXT px
  PRINT ""
NEXT py