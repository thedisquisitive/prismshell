# Prompt Customization

The shell assembles the prompt before each input:

1) **`prompt` mod** — full control.
2) **Template** (if set).
3) **Default**: `${status_emoji} ${shortcwd} pbsh> `

## Prompt Template

Set from BASIC or the shell:

```
CALL Prompt.SetTemplate("${status_emoji} ${shortcwd} [${time}] pbsh> ")
```

Placeholders:
- `${cwd}` — full working directory
- `${shortcwd}` — compact path (e.g., `/h/u/projects/repo`)
- `${status}` — last command’s status code
- `${status_emoji}` — ✅ (0) or ❌ (non-zero)
- `${time}` — HH:MM:SS

## Prompt Mod

Register a mod named `prompt` that produces a string (set `PROMPT` or `_`). It’s invoked as: `prompt --status <code>`.

```bas
REM mods/prompt.bas
10  CALL Mod.Register("prompt", 100)
20  END

100 LET CODE = PB_ARG2
110 CALL Env.Cwd() : LET C = _
120 IF CODE = "0" THEN 200
130 LET EMOJI = "❌" : GOTO 210
200 LET EMOJI = "✅"
210 LET PROMPT = EMOJI + " " + C + " pbsh> "
220 LET _ = PROMPT
230 END
```
