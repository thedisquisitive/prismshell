#!/usr/bin/env prismshell
' Test improved argv handling

PRINT "Command-line Arguments Test"
PRINT "============================"
PRINT ""

' Method 1: Using ARGV array directly
CALL ARR.SIZE("ARGV", 0)
LET argc = _
PRINT "Number of arguments: "; argc
PRINT ""

IF argc > 0 THEN
  PRINT "Arguments (using ARGV array):"
  FOR i = 0 TO argc - 1
    PRINT "  ARGV["; i; "] = '"; ARGV[i]; "'"
  NEXT i
  PRINT ""
ENDIF

' Method 2: Using ARGC()/ARGV() functions
CALL ARGC()
LET count = _
PRINT "Using ARGC() function: "; count

IF count > 0 THEN
  PRINT "Arguments (using ARGV function):"
  FOR i = 0 TO count - 1
    CALL ARGV(i)
    PRINT "  ARGV("; i; ") = '"; _; "'"
  NEXT i
  PRINT ""
ENDIF

' Method 3: Process arguments
IF argc >= 1 THEN
  PRINT "Processing first argument:"
  LET arg0 = ARGV[0]
  CALL UCASE(arg0)
  PRINT "  Uppercase: "; _
  CALL LEN(arg0)
  PRINT "  Length: "; _
ENDIF

' Method 4: Parse arguments with string functions
PRINT ""
PRINT "Argument analysis:"
FOR i = 0 TO argc - 1
  LET arg = ARGV[i]
  CALL INSTR(arg, "=")
  LET eq_pos = _
  
  IF eq_pos > 0 THEN
    CALL LEFT(arg, eq_pos - 1)
    LET key = _
    CALL MID(arg, eq_pos + 1)
    LET value = _
    PRINT "  Option: "; key; " = "; value
  ELSE
    PRINT "  Positional: "; arg
  ENDIF
NEXT i