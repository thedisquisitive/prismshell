# Mods (PrismBASIC Plugins)

Mods are BASIC files that **register commands**. They autoload from:

- `./mods`
- `~/.config/prismshell/mods`
- `/usr/local/share/prismshell/mods`

## Register a Command

```bas
REM mods/say.bas
10 CALL Mod.Register("say", 100)
20 END

100 IF PB_ARGC = 0 THEN 1000
110 PRINT "» " + PB_ARGS
120 END
1000 PRINT "usage: say <text>"
1010 END
```

In the shell:

```
pbsh> mods
say
pbsh> say "hello world"
» hello world
```

## Arguments You Get

- `PB_CMD` — command name
- `PB_ARGC` — count
- `PB_ARG1..PB_ARGN` — individual args
- `PB_ARGS` — space-joined args
- `PB_ARGV` — space-joined args (MVP)

> The shell’s tokenizer supports `"double"`, `'single'`, and backslash escapes.

## Mod Meta Commands

```
mods                 # list
mods list
mods enable <name>
mods disable <name>
mods reload
mods run <name> [args...]
```
