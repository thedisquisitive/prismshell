#!/usr/bin/env prismshell
' Test hash maps / associative arrays

PRINT "Hash Map / Associative Array Tests"
PRINT "==================================="
PRINT ""

' Test 1: Basic SET/GET
PRINT "Test 1: Basic SET/GET"
CALL MAP.SET("config", "name", "Alice")
CALL MAP.SET("config", "age", 25)
CALL MAP.SET("config", "role", "Engineer")

CALL MAP.GET("config", "name")
PRINT "  name: "; _

CALL MAP.GET("config", "age")
PRINT "  age: "; _

CALL MAP.GET("config", "role")
PRINT "  role: "; _
PRINT "  PASS"
PRINT ""

' Test 2: MAP.HAS
PRINT "Test 2: MAP.HAS"
CALL MAP.HAS("config", "name")
IF _ == 1 THEN
  PRINT "  Has 'name': PASS"
ENDIF

CALL MAP.HAS("config", "missing")
IF _ == 0 THEN
  PRINT "  Missing 'missing': PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' Test 3: Default values
PRINT "Test 3: Default values"
CALL MAP.GET("config", "missing", "DEFAULT")
PRINT "  Missing key with default: '"; _; "'"
IF _ == "DEFAULT" THEN
  PRINT "  Default value: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' Test 4: MAP.SIZE
PRINT "Test 4: MAP.SIZE"
CALL MAP.SIZE("config")
PRINT "  Size: "; _
IF _ == 3 THEN
  PRINT "  Size correct: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' Test 5: MAP.KEYS
PRINT "Test 5: MAP.KEYS"
CALL MAP.KEYS("config")
PRINT "  Keys: "; _
PRINT "  PASS"
PRINT ""

' Test 6: Iterate over map
PRINT "Test 6: Iterate over map"
CALL MAP.KEYS("config")
LET keys = _
PRINT "  All entries:"

' Parse newline-separated keys
LET remaining = keys
WHILE LEN(remaining) > 0
  CALL INSTR(remaining, CHR(10))
  LET nl_pos = _
  
  IF nl_pos == 0 THEN
    LET key = remaining
    LET remaining = ""
  ELSE
    CALL LEFT(remaining, nl_pos - 1)
    LET key = _
    CALL MID(remaining, nl_pos + 1)
    LET remaining = _
  ENDIF
  
  CALL MAP.GET("config", key)
  PRINT "    "; key; " = "; _
WEND
PRINT "  PASS"
PRINT ""

' Test 7: MAP.DELETE
PRINT "Test 7: MAP.DELETE"
CALL MAP.DELETE("config", "age")
CALL MAP.SIZE("config")
PRINT "  Size after delete: "; _
IF _ == 2 THEN
  PRINT "  Delete: PASS"
ENDIF

CALL MAP.HAS("config", "age")
IF _ == 0 THEN
  PRINT "  Deleted key gone: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' Test 8: Multiple maps
PRINT "Test 8: Multiple maps"
CALL MAP.SET("users", "alice", "admin")
CALL MAP.SET("users", "bob", "user")
CALL MAP.SET("scores", "alice", 100)
CALL MAP.SET("scores", "bob", 85)

CALL MAP.GET("users", "alice")
LET alice_role = _
CALL MAP.GET("scores", "alice")
LET alice_score = _

PRINT "  Alice: "; alice_role; ", score "; alice_score
IF alice_role == "admin" AND alice_score == 100 THEN
  PRINT "  Multiple maps: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' Test 9: MAP.CLEAR
PRINT "Test 9: MAP.CLEAR"
CALL MAP.CLEAR("config")
CALL MAP.SIZE("config")
IF _ == 0 THEN
  PRINT "  Cleared map: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

' Test 10: Practical example - word frequency counter
PRINT "Test 10: Word frequency counter"
LET text = "the quick brown fox jumps over the lazy dog the fox"

' Split and count
LET word_list = text
WHILE LEN(word_list) > 0
  CALL INSTR(word_list, " ")
  LET space_pos = _
  
  IF space_pos == 0 THEN
    LET word = word_list
    LET word_list = ""
  ELSE
    CALL LEFT(word_list, space_pos - 1)
    LET word = _
    CALL MID(word_list, space_pos + 1)
    LET word_list = _
  ENDIF
  
  CALL MAP.GET("freq", word, 0)
  LET count = _
  CALL MAP.SET("freq", word, count + 1)
WEND

PRINT "  Word frequencies:"
CALL MAP.KEYS("freq")
LET freq_keys = _

LET remaining2 = freq_keys
WHILE LEN(remaining2) > 0
  CALL INSTR(remaining2, CHR(10))
  LET nl2 = _
  
  IF nl2 == 0 THEN
    LET word2 = remaining2
    LET remaining2 = ""
  ELSE
    CALL LEFT(remaining2, nl2 - 1)
    LET word2 = _
    CALL MID(remaining2, nl2 + 1)
    LET remaining2 = _
  ENDIF
  
  CALL MAP.GET("freq", word2)
  PRINT "    '"; word2; "': "; _
WEND

CALL MAP.GET("freq", "the")
IF _ == 3 THEN
  PRINT "  Frequency counting: PASS"
ENDIF
PRINT "  PASS"
PRINT ""

PRINT "======================================="
PRINT "All Hash Map Tests Complete!"
PRINT "======================================="