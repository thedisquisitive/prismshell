10  CALL Mod.Register("prompt", 100)
20  END

100 REM Build a prompt string. Called as: prompt --status <code>
110 LET CODE = PB_ARG2
120 CALL Env.Cwd()
130 LET C = _
140 IF CODE = "0" THEN 200
150 LET EMOJI = "❌"
160 GOTO 210
200 LET EMOJI = "✅"
210 LET PROMPT = EMOJI + " " + C + " pbsh> "
220 LET _ = PROMPT
230 END
