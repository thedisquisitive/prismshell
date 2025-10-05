#!/usr/bin/env prismshell

LET iter = 0
LET x = 0
LET y = 0

PRINT "iter < 100: "; (iter < 100)
PRINT "x*x + y*y: "; (x*x + y*y)
PRINT "(x*x + y*y) < 4: "; ((x*x + y*y) < 4)
PRINT "Combined AND: "; (iter < 100 AND (x*x + y*y) < 4)

LET test = iter < 100 AND (x*x + y*y) < 4
PRINT "Assigned to variable: "; test