# Roadmap

Short-term
- Block control flow: `IF ... ELSE ... ENDIF`, `WHILE/WEND`, `FOR/NEXT`
- User `SUB ... END SUB` + `CALL Foo(...)`
- Arrays (`DIM`), `DATA/READ/RESTORE`
- Better argv object (`PB_ARGV` as array value, not string)
- Colors in prompt (Readline-safe wrappers)
- PrismFS verbs (`MOUNT.ADD`, `SNAPSHOT`, `BRANCH`) real backends

Longer-term
- Bytecode + small VM (debug hooks, breakpoints)
- Module system for CALL providers (C++ plugins)
- Tests & fuzzing for the parser
