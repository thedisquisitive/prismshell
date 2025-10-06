10  CALL Mod.Register("say", 100)
20  END

100 REM entry point
110 IF PB_ARGC = 0 THEN 1000
120 PRINT "» " + PB_ARGS
130 END

1000 PRINT "usage: say <text>"
1010 END
