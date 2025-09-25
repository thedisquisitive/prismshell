' ================================================
' PrismBASIC — Feature Demo (grammar-safe + blocks)
' ================================================

5   GOSUB 9000

10  PRINT "Welcome to the PrismBASIC feature demo."
20  PRINT

30  PRINT "Time now: ";
40  CALL Time.Now()
50  PRINT _

60  PRINT "Current directory: ";
70  CALL Env.Cwd()
80  PRINT _

90  GOTO 9100

' =======================
' Main menu + dispatcher
' =======================
9100 PRINT
9110 PRINT "Choose a demo:"
9120 PRINT "  1) PRINT & INPUT"
9130 PRINT "  2) IF ... THEN (single-line)"
9140 PRINT "  3) Variables & math"
9150 PRINT "  4) Random dice (RNG.Int)"
9160 PRINT "  5) GOSUB / RETURN"
9165 PRINT "  6) WHILE / WEND"
9167 PRINT "  7) IF / ELSEIF / ELSE / ENDIF (multiline)"
9170 PRINT "  8) Exit"
9180 PRINT
9190 PRINT "Enter choice (1-8): ";
9200 CALL TTY.ReadLine("")
9210 LET ch = _
' NOTE: single-line IF ... THEN <line> for dispatch
9220 IF ch = "1" THEN 1000
9225 IF ch = "2" THEN 2000
9230 IF ch = "3" THEN 3000
9235 IF ch = "4" THEN 4000
9240 IF ch = "5" THEN 5000
9243 IF ch = "6" THEN 6000
9244 IF ch = "7" THEN 7000
9245 IF ch = "8" THEN 9999
9250 GOTO 9100

' =========================
' 1) PRINT & INPUT demo
' =========================
1000 PRINT
1010 PRINT "PRINT demos:"
1020 PRINT "  a) Normal newline after the line"
1030 PRINT "  b) Trailing semicolon keeps cursor on same line -> ";
1040 PRINT "see? Now newline."
1050 PRINT
1060 PRINT "Enter your name: ";
1070 CALL TTY.ReadLine("")
1080 LET name = _
1090 PRINT "Hello, " + name + "!"
1100 GOTO 9100

' =================================
' 2) IF ... THEN (single-line) demo
' =================================
2000 PRINT
2010 PRINT "Type -1, 0, or 1: ";
2020 CALL TTY.ReadLine("")
2030 LET s = _
2040 IF s = "-1" THEN 2110
2045 IF s = "0" THEN 2130
2050 IF s = "1" THEN 2150
2060 PRINT "Not -1/0/1; treating as zero."
2070 GOTO 2130

2110 PRINT "That number is negative."
2120 GOTO 9100

2130 PRINT "That number is zero."
2140 GOTO 9100

2150 PRINT "That number is positive."
2160 GOTO 9100

' ==========================
' 3) Variables & math demo
' ==========================
3000 PRINT
3010 LET a = 2
3020 LET b = 3
3030 PRINT "a = "; a
3040 PRINT "b = "; b
3050 PRINT "a + b = "; a + b
3060 PRINT "a * b = "; a * b
3070 PRINT "a ^ 3 = "; a ^ 3
3080 GOTO 9100

' ==========================
' 4) RNG dice demo
' ==========================
4000 PRINT
4010 PRINT "Random dice roll:"
4020 CALL RNG.Int(1, 6)
4030 LET d = _
4040 PRINT "You rolled "; d
4050 IF d = 6 THEN 4090
4060 IF d = 1 THEN 4110
4070 PRINT "Solid roll."
4080 GOTO 9100
4090 PRINT "Nice! A perfect roll."
4100 GOTO 9100
4110 PRINT "Oof—snake eyes."
4120 GOTO 9100

' =====================================
' 5) GOSUB / RETURN (nested) demo
' =====================================
5000 PRINT
5010 PRINT "GOSUB demo: calling a subroutine twice..."
5020 GOSUB 5200
5030 GOSUB 5200
5040 GOTO 9100

5200 PRINT "  -> Hello from subroutine 5200 at ";
5210 CALL Time.Now()
5220 PRINT _
5230 RETURN

' ==========================
' 6) WHILE / WEND demo
' ==========================
6000 PRINT
6010 PRINT "WHILE/WEND demo: count from 1 to N"
6020 PRINT "Enter N (<= 10 recommended): ";
6030 CALL TTY.ReadLine("")
6040 LET s = _
6050 LET n = s
6060 IF n = 0 THEN 6100
6065 LET i = 1
6070 WHILE i <= n
6080   PRINT "  i = "; i
6085   LET i = i + 1
6090 WEND
6100 GOTO 9100

' ==========================================
' 7) IF / ELSEIF / ELSE / ENDIF (multiline)
' ==========================================
7000 PRINT
7010 PRINT "Multiline IF demo: enter a number (-1, 0, 1, other): ";
7020 CALL TTY.ReadLine("")
7030 LET s = _
7040 LET x = s
7050 IF x < 0 THEN
7060   PRINT "Branch: x < 0"
7070   PRINT "  Negative!"
7080 ELSEIF x = 0 THEN
7090   PRINT "Branch: x = 0"
7100   PRINT "  Zero!"
7110 ELSEIF x = 1 THEN
7120   PRINT "Branch: x = 1"
7130   PRINT "  One!"
7140 ELSE
7150   PRINT "Branch: else"
7160   PRINT "  Some other number: "; x
7170 ENDIF
7180 GOTO 9100

' ====================
' Banner / header
' ====================
9000 PRINT "========================================"
9010 PRINT " PrismBASIC — Feature Demo"
9020 PRINT "========================================"
9030 RETURN

' =====
' Exit
' =====
9999 PRINT
10000 PRINT "Thanks for trying the demo. Goodbye!"
10010 END
