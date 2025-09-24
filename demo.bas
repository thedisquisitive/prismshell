' ================================================
' PrismBASIC — Feature Demo (fits current runtime)
' ================================================
' Shows: PRINT/LET, CALL + "_", IF/ELSEIF/ELSE,
'        WHILE/WEND, GOSUB/RETURN, GOTO,
'        trailing ";" to suppress newline,
'        Env.Cwd, TTY.ReadLine, RNG.Int, Time.Now

5   GOSUB 9000

10  PRINT "Welcome to the PrismBASIC feature demo."
20  PRINT

' --- Builtins via CALL (return captured in _) ---
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
9130 PRINT "  2) IF / ELSEIF / ELSE"
9140 PRINT "  3) WHILE / WEND"
9150 PRINT "  4) Random dice (RNG.Int)"
9160 PRINT "  5) GOSUB / RETURN"
9170 PRINT "  6) Exit"
9180 PRINT
9190 PRINT "Enter choice (1-6): ";
9200 CALL TTY.ReadLine("")
9210 LET ch = _
9220 IF ch = "1" THEN GOSUB 1000
9230 ELSEIF ch = "2" THEN GOSUB 2000
9240 ELSEIF ch = "3" THEN GOSUB 3000
9250 ELSEIF ch = "4" THEN GOSUB 4000
9260 ELSEIF ch = "5" THEN GOSUB 5000
9270 ELSEIF ch = "6" THEN GOTO 9999
9280 ELSE PRINT "Unknown choice: " + ch
9290 ENDIF
9300 GOTO 9100


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
1100 RETURN


' =================================
' 2) IF / ELSEIF / ELSE demo
' (keeps it simple on parsing)
' =================================
2000 PRINT
2010 PRINT "IF / ELSEIF / ELSE demo"
2020 PRINT "Type -1, 0, or 1: ";
2030 CALL TTY.ReadLine("")
2040 LET s = _
2050 LET x = 0
2060 IF s = "-1" THEN LET x = -1
2070 ELSEIF s = "0" THEN LET x = 0
2080 ELSEIF s = "1" THEN LET x = 1
2090 ELSE LET x = 0
2100 ENDIF
2110 IF x < 0 THEN
2120   PRINT "That number is negative."
2130 ELSEIF x = 0 THEN
2140   PRINT "That number is zero."
2150 ELSE
2160   PRINT "That number is positive."
2170 ENDIF
2180 RETURN


' ==========================
' 3) WHILE / WEND demo
' ==========================
3000 PRINT
3010 PRINT "WHILE/WEND demo: counting to 5"
3020 LET i = 1
3030 WHILE i <= 5
3040   PRINT "  i = "; i
3050   LET i = i + 1
3060 WEND
3070 RETURN


' ==========================
' 4) RNG dice demo
' ==========================
4000 PRINT
4010 PRINT "Random dice roll:"
4020 CALL RNG.Int(1, 6)
4030 LET d = _
4040 PRINT "You rolled "; d
4050 IF d = 6 THEN
4060   PRINT "Nice! A perfect roll."
4070 ELSEIF d = 1 THEN
4080   PRINT "Oof—snake eyes."
4090 ELSE
4100   PRINT "Solid roll."
4110 ENDIF
4120 RETURN


' =====================================
' 5) GOSUB / RETURN (nested) demo
' =====================================
5000 PRINT
5010 PRINT "GOSUB demo: calling a subroutine twice..."
5020 GOSUB 5200
5030 GOSUB 5200
5040 RETURN

5200 PRINT "  -> Hello from subroutine 5200 at ";
5210 CALL Time.Now()
5220 PRINT _
5230 RETURN


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
