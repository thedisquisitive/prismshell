#!/usr/bin/env prismshell
' color_demo.bas - Demonstrate ANSI color support

PRINT "========================================="
PRINT "PrismBASIC Color Demonstration"
PRINT "========================================="
PRINT ""

' Check if terminal supports colors
CALL Color.Supported()
IF _ == 0 THEN
  PRINT "Warning: Your terminal may not support colors"
  PRINT ""
ENDIF

' Test 1: Basic colors
PRINT "Test 1: Basic Colors"
PRINT "--------------------"

CALL Color.Red()
PRINT _; "  Red text"

CALL Color.Green()
PRINT _; "  Green text"

CALL Color.Yellow()
PRINT _; "  Yellow text"

CALL Color.Blue()
PRINT _; "  Blue text"

CALL Color.Magenta()
PRINT _; "  Magenta text"

CALL Color.Cyan()
PRINT _; "  Cyan text"

CALL Color.White()
PRINT _; "  White text"

CALL Color.Reset()
PRINT _; "  Reset to default"
PRINT ""

' Test 2: Bright colors
PRINT "Test 2: Bright Colors"
PRINT "---------------------"

CALL Color.Bright("red")
PRINT _; "  Bright red"

CALL Color.Bright("green")
PRINT _; "  Bright green"

CALL Color.Bright("yellow")
PRINT _; "  Bright yellow"

CALL Color.Bright("blue")
PRINT _; "  Bright blue"

CALL Color.Reset()
PRINT _
PRINT ""

' Test 3: Background colors
PRINT "Test 3: Background Colors"
PRINT "-------------------------"

CALL Color("black", 1)
LET bg = _
CALL Color.White()
LET fg = _
CALL Color.Reset()
LET reset = _
PRINT fg; bg; " Black background "; reset

CALL Color("red", 1)
LET bg = _
CALL Color.White()
LET fg = _
PRINT fg; bg; " Red background "; reset

CALL Color("green", 1)
LET bg = _
CALL Color("black")
LET fg = _
PRINT fg; bg; " Green background "; reset

CALL Color("blue", 1)
LET bg = _
CALL Color.White()
LET fg = _
PRINT fg; bg; " Blue background "; reset

PRINT ""

' Test 4: Formatting
PRINT "Test 4: Text Formatting"
PRINT "-----------------------"

CALL Color.Bold()
PRINT _; "  Bold text"

CALL Color.Reset()
CALL Color.Dim()
PRINT _; "  Dim text"

CALL Color.Reset()
CALL Color.Underline()
PRINT _; "  Underlined text"

CALL Color.Reset()
PRINT _
PRINT ""

' Test 5: Combined effects
PRINT "Test 5: Combined Effects"
PRINT "------------------------"

CALL Color.Bold()
LET bold = _
CALL Color.Red()
LET red = _
CALL Color.Reset()
LET reset = _
PRINT bold; red; "  Bold red text"; reset

CALL Color.Underline()
LET under = _
CALL Color.Green()
LET green = _
PRINT under; green; "  Underlined green text"; reset

CALL Color("yellow", 1)
LET bg = _
CALL Color.Bold()
LET bold = _
CALL Color("black")
LET black = _
PRINT black; bold; bg; " Bold black on yellow "; reset

PRINT ""

' Test 6: RGB colors (24-bit true color)
PRINT "Test 6: RGB True Color"
PRINT "----------------------"
PRINT "(May not work on all terminals)"

CALL Color.RGB(255, 0, 0)
PRINT _; "  Pure red (255,0,0)"

CALL Color.RGB(0, 255, 0)
PRINT _; "  Pure green (0,255,0)"

CALL Color.RGB(0, 0, 255)
PRINT _; "  Pure blue (0,0,255)"

CALL Color.RGB(255, 128, 0)
PRINT _; "  Orange (255,128,0)"

CALL Color.RGB(128, 0, 255)
PRINT _; "  Purple (128,0,255)"

CALL Color.RGB(0, 255, 255)
PRINT _; "  Cyan (0,255,255)"

CALL Color.Reset()
PRINT _
PRINT ""

' Test 7: 256-color palette
PRINT "Test 7: 256-Color Palette Sample"
PRINT "---------------------------------"
PRINT "(May not work on all terminals)"

FOR i = 0 TO 15
  CALL Color.Palette(i)
  PRINT _; "â–ˆ";
NEXT i
CALL Color.Reset()
PRINT _
PRINT ""

' Test 8: Practical examples
PRINT "Test 8: Practical Examples"
PRINT "--------------------------"

' Success message
CALL Color.Bold()
LET bold = _
CALL Color.Green()
LET green = _
CALL Color.Reset()
LET reset = _
PRINT bold; green; "SUCCESS:"; reset; " Operation completed"

' Error message
CALL Color.Bold()
LET bold = _
CALL Color.Red()
LET red = _
PRINT bold; red; "ERROR:"; reset; " File not found"

' Warning message
CALL Color.Bold()
LET bold = _
CALL Color.Yellow()
LET yellow = _
PRINT bold; yellow; "WARNING:"; reset; " Low disk space"

' Info message
CALL Color.Bold()
LET bold = _
CALL Color.Cyan()
LET cyan = _
PRINT bold; cyan; "INFO:"; reset; " Update available"

PRINT ""

' Test 9: Syntax highlighting example
PRINT "Test 9: Syntax Highlighting Demo"
PRINT "---------------------------------"

CALL Color("blue")
LET keyword = _
CALL Color("green")
LET string = _
CALL Color("magenta")
LET number = _
CALL Color.Reset()
LET reset = _

PRINT keyword; "FOR"; reset; " i "; keyword; "="; reset; " "; number; "1"; reset; " "; keyword; "TO"; reset; " "; number; "10"
PRINT "  "; keyword; "PRINT"; reset; " "; string; '\"Hello World\"'; reset
PRINT keyword; "NEXT"; reset; " i"

PRINT ""

' Test 10: Progress bar
PRINT "Test 10: Colored Progress Bar"
PRINT "------------------------------"

LET progress = 7
LET total = 10

CALL Color("green", 1)
LET filled = _
CALL Color("white")
LET fg = _
CALL Color.Reset()
LET reset = _

PRINT "Progress: [";
FOR i = 1 TO progress
  PRINT fg; filled; "â–ˆ"; reset;
NEXT i
FOR i = progress + 1 TO total
  PRINT "â–'";
NEXT i
PRINT "] "; progress * 10; "%"

PRINT ""

' Test 11: Color gradient (using RGB)
PRINT "Test 11: RGB Gradient"
PRINT "---------------------"

FOR i = 0 TO 20
  LET r = i * 12
  LET g = 255 - (i * 12)
  LET b = 128
  CALL Color.RGB(r, g, b)
  PRINT _; "â–ˆ";
NEXT i
CALL Color.Reset()
PRINT _
PRINT ""

' Final reset
CALL Color.Reset()
PRINT _

PRINT "========================================="
PRINT "Color Demo Complete!"
PRINT "========================================="
END