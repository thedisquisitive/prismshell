#!/usr/bin/env prismshell
REM PrismBASIC Script
10 PRINT "PrismBASIC v0.1 — type HELP"
20 CALL TTY.ReadLine("Name? ")
30 LET NAME = _
40 PRINT "Hello, "+NAME
50 CALL Env.Cwd()
60 PRINT "CWD="+_
70 PRINT "Try: ls; echo hi; or type BYE to quit"