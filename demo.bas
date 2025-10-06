#!/usr/bin/env prismshell
' ================================================
' PrismBASIC — Comprehensive Feature Showcase
' Demonstrates: Arrays, Hash Maps, SUBs, Colors,
' Control Flow, String Processing, File I/O
' ================================================

5   GOSUB 9000

' Check color support
10  CALL Color.Supported()
20  LET USE_COLOR = _

' Set up color codes if available
30  IF USE_COLOR == 1 THEN
40    CALL Color.Bold()
50    LET BOLD = _
60    CALL Color.Cyan()
70    LET CYAN = _
80    CALL Color.Green()
90    LET GREEN = _
100   CALL Color.Yellow()
110   LET YELLOW = _
120   CALL Color.Magenta()
130   LET MAGENTA = _
140   CALL Color.Red()
150   LET RED = _
160   CALL Color.Reset()
170   LET RESET = _
180 ELSE
190   LET BOLD = ""
200   LET CYAN = ""
210   LET GREEN = ""
220   LET YELLOW = ""
230   LET MAGENTA = ""
240   LET RED = ""
250   LET RESET = ""
260 ENDIF

' Display environment info
270 PRINT BOLD; CYAN; "System Information:"; RESET
280 CALL Env.Cwd()
290 PRINT "  Working Directory: "; GREEN; _; RESET
300 CALL Time.Now()
310 PRINT "  Current Time: "; YELLOW; _; RESET
320 PRINT ""

330 GOTO 9100

' =======================
' Main menu + dispatcher
' =======================
9100 PRINT ""
9110 PRINT BOLD; YELLOW; "PrismBASIC Feature Demos:"; RESET
9120 PRINT "  1) Multidimensional Arrays & Matrix Math"
9130 PRINT "  2) Hash Maps & Data Processing"
9140 PRINT "  3) String Manipulation & Parsing"
9150 PRINT "  4) Recursive SUBs (Fibonacci & Factorial)"
9160 PRINT "  5) File I/O & Directory Operations"
9170 PRINT "  6) Graphics Demo (ASCII Art Animation)"
9180 PRINT "  7) Mini Text Adventure Game"
9190 PRINT "  8) System Integration (RNG, Time, Env)"
9200 PRINT "  9) Data Analysis (CSV Processing)"
9210 PRINT "  0) Exit"
9220 PRINT ""
9230 PRINT CYAN; "Enter choice (0-9): "; RESET;
9240 CALL TTY.ReadLine("")
9250 LET ch = _

9260 IF ch == "1" THEN 1000
9270 IF ch == "2" THEN 2000
9280 IF ch == "3" THEN 3000
9290 IF ch == "4" THEN 4000
9300 IF ch == "5" THEN 5000
9310 IF ch == "6" THEN 6000
9320 IF ch == "7" THEN 7000
9330 IF ch == "8" THEN 8000
9340 IF ch == "9" THEN 10000
9350 IF ch == "0" THEN 9999
9360 PRINT RED; "Invalid choice!"; RESET
9370 GOTO 9100

' =========================
' 1) Multidimensional Arrays & Matrix Math
' =========================
1000 PRINT ""
1010 PRINT BOLD; CYAN; "=== Multidimensional Array Demo ==="; RESET
1020 PRINT ""

1030 PRINT "Creating 3x3 matrix..."
1040 DIM matrix[3, 3]

1050 PRINT "Populating with multiplication table:"
1060 FOR row = 0 TO 2
1070   FOR col = 0 TO 2
1080     LET matrix[row, col] = (row + 1) * (col + 1)
1090   NEXT col
1100 NEXT row

1110 PRINT ""
1120 PRINT "Matrix contents:"
1130 FOR row = 0 TO 2
1140   PRINT "  ";
1150   FOR col = 0 TO 2
1160     PRINT YELLOW; matrix[row, col]; RESET; "  ";
1170   NEXT col
1180   PRINT ""
1190 NEXT row

1200 PRINT ""
1210 PRINT "Matrix sum:";
1220 LET sum = 0
1230 FOR row = 0 TO 2
1240   FOR col = 0 TO 2
1250     LET sum = sum + matrix[row, col]
1260   NEXT col
1270 NEXT row
1280 PRINT " "; GREEN; sum; RESET

1290 PRINT ""
1300 PRINT "Creating dynamic 3D array (sparse data)..."
1310 DIM cube[, , ]
1320 LET cube[0, 0, 0] = 111
1330 LET cube[5, 5, 5] = 555
1340 LET cube[9, 9, 9] = 999

1350 PRINT "Corner values:"
1360 PRINT "  cube[0,0,0] = "; CYAN; cube[0, 0, 0]; RESET
1370 PRINT "  cube[5,5,5] = "; MAGENTA; cube[5, 5, 5]; RESET
1380 PRINT "  cube[9,9,9] = "; YELLOW; cube[9, 9, 9]; RESET

1390 CALL ARR.DIMS("cube")
1400 PRINT ""
1410 PRINT "Array dimensions: "; GREEN; _; RESET
1420 CALL ARR.SIZE("cube", 0)
1430 PRINT "Size[0]: "; _
1440 CALL ARR.SIZE("cube", 1)
1450 PRINT "Size[1]: "; _
1460 CALL ARR.SIZE("cube", 2)
1470 PRINT "Size[2]: "; _

1480 PRINT ""
1490 PRINT GREEN; "Press Enter to continue..."; RESET
1500 CALL TTY.ReadLine("")
1510 GOTO 9100

' =========================
' 2) Hash Maps & Data Processing
' =========================
2000 PRINT ""
2010 PRINT BOLD; CYAN; "=== Hash Map Demo ==="; RESET
2020 PRINT ""

2030 PRINT "Building student database..."
2040 CALL Map.Set("students.alice", "grade", "95")
2050 CALL Map.Set("students.alice", "major", "CS")
2060 CALL Map.Set("students.alice", "year", "3")

2070 CALL Map.Set("students.bob", "grade", "87")
2080 CALL Map.Set("students.bob", "major", "Math")
2090 CALL Map.Set("students.bob", "year", "2")

2100 CALL Map.Set("students.charlie", "grade", "92")
2110 CALL Map.Set("students.charlie", "major", "Physics")
2120 CALL Map.Set("students.charlie", "year", "4")

2130 PRINT ""
2140 PRINT "Student Records:"
2150 PRINT ""

2160 REM Display Alice
2170 PRINT YELLOW; "Alice:"; RESET
2180 CALL Map.Get("students.alice", "major")
2190 PRINT "  Major: "; _
2200 CALL Map.Get("students.alice", "grade")
2210 PRINT "  Grade: "; GREEN; _; RESET
2220 CALL Map.Get("students.alice", "year")
2230 PRINT "  Year: "; _

2240 REM Display Bob
2250 PRINT ""
2260 PRINT YELLOW; "Bob:"; RESET
2270 CALL Map.Get("students.bob", "major")
2280 PRINT "  Major: "; _
2290 CALL Map.Get("students.bob", "grade")
2300 PRINT "  Grade: "; CYAN; _; RESET
2310 CALL Map.Get("students.bob", "year")
2320 PRINT "  Year: "; _

2330 REM Display Charlie
2340 PRINT ""
2350 PRINT YELLOW; "Charlie:"; RESET
2360 CALL Map.Get("students.charlie", "major")
2370 PRINT "  Major: "; _
2380 CALL Map.Get("students.charlie", "grade")
2390 PRINT "  Grade: "; GREEN; _; RESET
2400 CALL Map.Get("students.charlie", "year")
2410 PRINT "  Year: "; _

2420 PRINT ""
2430 PRINT "Word frequency counter demo:"
2440 LET text = "the quick brown fox jumps over the lazy dog the end"
2450 PRINT "Text: '"; MAGENTA; text; RESET; "'"

2460 REM Count "the"
2470 CALL Map.Clear("freq")
2480 LET word_count = 0
2490 LET search_pos = 1
2500 CALL LEN(text)
2510 LET text_len = _

2520 WHILE search_pos <= text_len
2530   CALL MID(text, search_pos)
2540   CALL INSTR(_, "the")
2550   IF _ > 0 THEN
2560     LET word_count = word_count + 1
2570     LET search_pos = search_pos + _ + 2
2580   ELSE
2590     LET search_pos = text_len + 1
2600   ENDIF
2610 WEND

2620 PRINT ""
2630 PRINT "Occurrences of 'the': "; GREEN; word_count; RESET

2640 PRINT ""
2650 PRINT GREEN; "Press Enter to continue..."; RESET
2660 CALL TTY.ReadLine("")
2670 GOTO 9100

' =========================
' 3) String Manipulation & Parsing
' =========================
3000 PRINT ""
3010 PRINT BOLD; CYAN; "=== String Processing Demo ==="; RESET
3020 PRINT ""

3030 LET sample = "  Hello, PrismBASIC World!  "
3040 PRINT "Original: '"; YELLOW; sample; RESET; "'"

3050 CALL TRIM(sample)
3060 LET trimmed = _
3070 PRINT "Trimmed:  '"; GREEN; trimmed; RESET; "'"

3080 CALL UCASE(trimmed)
3090 PRINT "Upper:    '"; CYAN; _; RESET; "'"

3100 CALL LCASE(trimmed)
3110 PRINT "Lower:    '"; MAGENTA; _; RESET; "'"

3120 PRINT ""
3130 PRINT "Substring extraction:"
3140 CALL LEFT(trimmed, 5)
3150 PRINT "  LEFT(5):  '"; GREEN; _; RESET; "'"
3160 CALL RIGHT(trimmed, 6)
3170 PRINT "  RIGHT(6): '"; CYAN; _; RESET; "'"
3180 CALL MID(trimmed, 8, 10)
3190 PRINT "  MID(8,10): '"; YELLOW; _; RESET; "'"

3200 PRINT ""
3210 PRINT "Searching:"
3220 CALL INSTR(trimmed, "BASIC")
3230 PRINT "  Position of 'BASIC': "; GREEN; _; RESET

3240 PRINT ""
3250 PRINT "CSV Parsing demo:"
3260 LET csv = "Alice,30,Engineer,NYC"
3270 PRINT "Data: "; MAGENTA; csv; RESET

3280 REM Parse CSV - find each comma position
3290 DIM fields[4]

3300 REM Find first comma
3310 CALL INSTR(csv, ",")
3320 LET c1 = _
3330 CALL LEFT(csv, c1 - 1)
3340 LET fields[0] = _

3350 REM Find second comma (search from after first comma)
3360 CALL MID(csv, c1 + 1)
3370 CALL INSTR(_, ",")
3380 LET c2 = _
3390 CALL MID(csv, c1 + 1, c2 - 1)
3400 LET fields[1] = _

3410 REM Find third comma
3420 CALL MID(csv, c1 + c2 + 1)
3430 CALL INSTR(_, ",")
3440 LET c3 = _
3450 CALL MID(csv, c1 + c2 + 1, c3 - 1)
3460 LET fields[2] = _

3470 REM Get last field (after third comma)
3480 CALL MID(csv, c1 + c2 + c3 + 1)
3490 LET fields[3] = _

3500 PRINT ""
3510 PRINT "Parsed fields:"
3520 PRINT "  Name: "; YELLOW; fields[0]; RESET
3530 PRINT "  Age: "; GREEN; fields[1]; RESET
3540 PRINT "  Job: "; CYAN; fields[2]; RESET
3550 PRINT "  City: "; MAGENTA; fields[3]; RESET

3560 PRINT ""
3570 PRINT GREEN; "Press Enter to continue..."; RESET
3580 CALL TTY.ReadLine("")
3590 GOTO 9100
' =========================
' 4) Recursive SUBs
' =========================
4000 PRINT ""
4010 PRINT BOLD; CYAN; "=== Recursive Subroutines Demo ==="; RESET
4020 PRINT ""

4030 PRINT "Fibonacci sequence (first 15 numbers):"
4040 PRINT "  ";
4050 FOR i = 0 TO 14
4060   CALL Fibonacci(i)
4070   LET fib_result = _
4080   PRINT YELLOW; fib_result; RESET; " ";
4090 NEXT i

4100 PRINT ""
4110 PRINT ""
4120 PRINT "Factorial calculations:"
4130 FOR i = 1 TO 10
4140   CALL Factorial(i)
4150   LET fac_result = _
4160   PRINT "  "; i; "! = "; GREEN; fac_result; RESET
4170 NEXT i

4180 PRINT ""
4190 PRINT "Power function (2^n):"
4200 FOR i = 0 TO 10
4210   CALL Power(2, i)
4220   LET pow_result = _
4230   PRINT "  2^"; i; " = "; CYAN; pow_result; RESET
4240 NEXT i

4250 PRINT ""
4260 PRINT GREEN; "Press Enter to continue..."; RESET
4270 CALL TTY.ReadLine("")
4280 GOTO 9100

' =========================
' 5) File I/O & Directory Operations
' =========================
5000 PRINT ""
5010 PRINT BOLD; CYAN; "=== File I/O Demo ==="; RESET
5020 PRINT ""

5030 LET testfile = "prism_demo_test.txt"
5040 LET content = "PrismBASIC File I/O Test\nLine 2\nLine 3"

5050 PRINT "Writing to "; YELLOW; testfile; RESET; "..."
5060 CALL FS.Write(testfile, content)

5070 PRINT "Reading file back..."
5080 CALL FS.Read(testfile)
5090 PRINT ""
5100 PRINT GREEN; _; RESET
5110 PRINT ""

5120 PRINT "File size:"
5130 CALL FS.Size(testfile)
5140 PRINT "  "; CYAN; _; RESET; " bytes"

5150 PRINT ""
5160 PRINT "Appending data..."
5170 CALL FS.Append(testfile, "\nAppended line")

5180 PRINT "New content:"
5190 CALL FS.Read(testfile)
5200 PRINT ""
5210 PRINT GREEN; _; RESET

5220 PRINT ""
5230 PRINT "Directory operations:"
5240 LET testdir = "prism_demo_dir"
5250 PRINT "  Creating directory: "; YELLOW; testdir; RESET
5260 CALL FS.Mkdir(testdir)

5270 PRINT "  Touching file in directory..."
5280 CALL Path.Join(testdir, "sample.txt")
5290 CALL FS.Touch(_)

5300 PRINT "  Listing directory:"
5310 CALL FS.List(testdir)
5320 PRINT "    "; GREEN; _; RESET

5330 PRINT ""
5340 PRINT "Cleaning up..."
5350 CALL FS.Delete(testfile)
5360 CALL FS.RemoveAll(testdir)
5370 PRINT GREEN; "  Done!"; RESET

5380 PRINT ""
5390 PRINT GREEN; "Press Enter to continue..."; RESET
5400 CALL TTY.ReadLine("")
5410 GOTO 9100

' =========================
' 6) Graphics Demo (ASCII Art Animation)
' =========================
6000 PRINT ""
6010 PRINT BOLD; CYAN; "=== ASCII Animation Demo ==="; RESET
6020 PRINT ""

6030 PRINT "Animated sine wave (Press Ctrl-C to stop):"
6040 PRINT ""

6050 FOR frame = 1 TO 30
6060   LET line = ""
6070   FOR x = 0 TO 60
6080     LET angle = (x + frame) * 0.2
6090     CALL Sin(angle)
6100     LET y = _ * 10 + 12
6110     
6120     IF y >= 0 AND y < 25 THEN
6130       IF USE_COLOR == 1 THEN
6140         IF y < 8 THEN
6150           CALL Color.Red()
6160         ELSEIF y < 17 THEN
6170           CALL Color.Yellow()
6180         ELSE
6190           CALL Color.Green()
6200         ENDIF
6210         LET line = line + _
6220       ENDIF
6230       LET line = line + "*"
6240       IF USE_COLOR == 1 THEN
6250         CALL Color.Reset()
6260         LET line = line + _
6270       ENDIF
6280     ENDIF
6290   NEXT x
6300   
6310   PRINT line
6320 NEXT frame

6330 PRINT ""
6340 PRINT GREEN; "Press Enter to continue..."; RESET
6350 CALL TTY.ReadLine("")
6360 GOTO 9100

' =========================
' 7) Mini Text Adventure Game
' =========================
7000 PRINT ""
7010 PRINT BOLD; CYAN; "=== Text Adventure: The Cave ==="; RESET
7020 PRINT ""

7030 REM Initialize game state
7040 CALL Map.Set("game", "location", "entrance")
7050 CALL Map.Set("game", "has_torch", "0")
7060 CALL Map.Set("game", "has_key", "0")
7070 LET playing = 1

7080 PRINT "You stand at the entrance of a dark cave."
7090 PRINT "Commands: north, south, east, west, take, use, look, quit"
7100 PRINT ""

7110 WHILE playing == 1
7120   CALL Map.Get("game", "location")
7130   LET loc = _
7140   
7150   PRINT ""
7160   PRINT YELLOW; "> "; RESET;
7170   CALL TTY.ReadLine("")
7180   LET cmd = _
7190   CALL LCASE(cmd)
7200   LET cmd = _
7210   
7220   IF cmd == "quit" THEN
7230     PRINT "Thanks for playing!"
7240     LET playing = 0
7250     GOTO 7110
7260   ENDIF
7270   
7280   IF cmd == "look" THEN
7290     CALL DescribeLocation(loc)
7300     GOTO 7110
7310   ENDIF
7320   
7330   IF cmd == "north" OR cmd == "south" OR cmd == "east" OR cmd == "west" THEN
7340     CALL Move(loc, cmd)
7350     CALL Map.Set("game", "location", _)
7360     CALL Map.Get("game", "location")
7370     CALL DescribeLocation(_)
7380     GOTO 7110
7390   ENDIF
7400   
7410   IF cmd == "take torch" AND loc == "entrance" THEN
7420     CALL Map.Set("game", "has_torch", "1")
7430     PRINT GREEN; "You take the torch. It lights up!"; RESET
7440     GOTO 7110
7450   ENDIF
7460   
7470   IF cmd == "take key" AND loc == "treasure" THEN
7480     CALL Map.Set("game", "has_key", "1")
7490     PRINT GREEN; "You take the golden key!"; RESET
7500     GOTO 7110
7510   ENDIF
7520   
7530   PRINT RED; "I don't understand that command."; RESET
7540 WEND

7550 PRINT ""
7560 PRINT GREEN; "Press Enter to continue..."; RESET
7570 CALL TTY.ReadLine("")
7580 GOTO 9100

' =========================
' 8) System Integration
' =========================
8000 PRINT ""
8010 PRINT BOLD; CYAN; "=== System Integration Demo ==="; RESET
8020 PRINT ""

8030 PRINT "Random Number Generation:"
8040 CALL RNG.Seed()
8050 PRINT "  Rolling dice (10 times):"
8060 PRINT "  ";
8070 FOR i = 1 TO 10
8080   CALL RNG.Int(1, 6)
8090   PRINT YELLOW; _; RESET; " ";
8100 NEXT i

8110 PRINT ""
8120 PRINT ""
8130 PRINT "  Random floats (5 samples):"
8140 PRINT "  ";
8150 FOR i = 1 TO 5
8160   CALL RNG.Float()
8170   PRINT GREEN; _; RESET; " ";
8180 NEXT i

8190 PRINT ""
8200 PRINT ""
8210 PRINT "Environment Variables:"
8220 CALL Env.Get("HOME")
8230 PRINT "  HOME: "; CYAN; _; RESET
8240 CALL Env.Get("USER")
8250 PRINT "  USER: "; YELLOW; _; RESET
8260 CALL Env.Get("SHELL")
8270 PRINT "  SHELL: "; GREEN; _; RESET

8280 PRINT ""
8290 PRINT "Time Functions:"
8300 CALL Time.Now()
8310 PRINT "  Current time: "; MAGENTA; _; RESET

8320 PRINT ""
8330 PRINT "Path Manipulation:"
8340 LET base = "/home/user/documents"
8350 LET file = "report.txt"
8360 PRINT "  Base: "; CYAN; base; RESET
8370 PRINT "  File: "; YELLOW; file; RESET
8380 CALL Path.Join(base, file)
8390 PRINT "  Joined: "; GREEN; _; RESET
8400 CALL Path.Dirname(_)
8410 PRINT "  Dirname: "; MAGENTA; _; RESET
8420 CALL Path.Join(base, file)
8430 CALL Path.Basename(_)
8440 PRINT "  Basename: "; CYAN; _; RESET
8450 CALL Path.Join(base, file)
8460 CALL Path.Ext(_)
8470 PRINT "  Extension: "; YELLOW; _; RESET

8480 PRINT ""
8490 PRINT GREEN; "Press Enter to continue..."; RESET
8500 CALL TTY.ReadLine("")
8510 GOTO 9100

' ====================
' Banner / header
' ====================
9000 PRINT BOLD; CYAN; "========================================"
9010 PRINT " PrismBASIC — Comprehensive Demo"
9020 PRINT "========================================"; RESET
9030 PRINT ""
9040 RETURN

' =====
' Exit
' =====
9999 PRINT ""
10000 PRINT BOLD; GREEN; "Thanks for exploring PrismBASIC!"; RESET
10010 PRINT ""
10020 END

' =====================
' Subroutine Definitions
' =====================

SUB Fibonacci(n)
  IF n <= 1 THEN
    LET _ = n
  ELSE
    CALL Fibonacci(n - 1)
    LET prev = _
    CALL Fibonacci(n - 2)
    LET _ = prev + _
  ENDIF
END SUB

SUB Factorial(n)
  IF n <= 1 THEN
    LET _ = 1
  ELSE
    CALL Factorial(n - 1)
    LET _ = n * _
  ENDIF
END SUB

SUB Power(base, exp)
  IF exp == 0 THEN
    LET _ = 1
  ELSE
    CALL Power(base, exp - 1)
    LET _ = base * _
  ENDIF
END SUB

SUB Sin(x)
  REM Approximation: sin(x) ≈ x - x³/6 + x⁵/120
  LET x2 = x * x
  LET x3 = x2 * x
  LET x5 = x3 * x2
  LET _ = x - (x3 / 6) + (x5 / 120)
END SUB

SUB DescribeLocation(loc)
  IF loc == "entrance" THEN
    PRINT ""
    PRINT "You are at the cave entrance."
    PRINT "A torch lies on the ground."
    PRINT "Paths lead: north (deeper), south (outside)"
  ELSEIF loc == "outside" THEN
    PRINT ""
    PRINT "You exit the cave into sunlight."
    PRINT "Path leads: north (back to entrance)"
  ELSEIF loc == "depths" THEN
    CALL Map.Get("game", "has_torch")
    IF _ == "0" THEN
      PRINT ""
      PRINT "It's too dark to see! You need light."
    ELSE
      PRINT ""
      PRINT "Your torch illuminates ancient carvings."
      PRINT "Paths: south (entrance), east (chamber)"
    ENDIF
  ELSEIF loc == "treasure" THEN
    PRINT ""
    PRINT "A chamber filled with gold!"
    PRINT "A golden key sits on a pedestal."
    PRINT "Path: west (depths)"
  ELSE
    PRINT ""
    PRINT "You are somewhere in the cave."
  ENDIF
END SUB

SUB Move(from, direction)
  IF from == "entrance" THEN
    IF direction == "north" THEN
      LET _ = "depths"
    ELSEIF direction == "south" THEN
      LET _ = "outside"
    ELSE
      PRINT "You can't go that way!"
      LET _ = from
    ENDIF
  ELSEIF from == "outside" THEN
    IF direction == "north" THEN
      LET _ = "entrance"
    ELSE
      PRINT "You can't go that way!"
      LET _ = from
    ENDIF
  ELSEIF from == "depths" THEN
    CALL Map.Get("game", "has_torch")
    IF _ == "0" THEN
      PRINT "Too dark to move! Get the torch first."
      LET _ = from
    ELSE
      IF direction == "south" THEN
        LET _ = "entrance"
      ELSEIF direction == "east" THEN
        LET _ = "treasure"
      ELSE
        PRINT "You can't go that way!"
        LET _ = from
      ENDIF
    ENDIF
  ELSEIF from == "treasure" THEN
    IF direction == "west" THEN
      LET _ = "depths"
    ELSE
      PRINT "You can't go that way!"
      LET _ = from
    ENDIF
  ELSE
    LET _ = from
  ENDIF
END SUB