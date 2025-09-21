# Getting Started

## Direct Mode

Type BASIC statements and they execute immediately:

```
PRINT "Hello"
CALL Env.Cwd()
PRINT _
```

`_` holds the result of the last `CALL`.

## Line-numbered Editor

```
10 PRINT "PrismBASIC v0.1 — type HELP"
20 INPUT A
30 PRINT "You typed " + A
LIST
RUN
SAVE demo.bas
NEW
LOAD demo.bas
```

- `LIST` prints the current program.
- `RUN` executes the line-numbered program.
- `SAVE/LOAD` persist/restore (`SAVE foo.bas`, `LOAD foo.bas`).
- Typing a bare line number deletes that line.

## Shebang Scripts

```bas
#!/usr/bin/env prismshell
10 PRINT "Args:"
20 CALL Env.Args()
30 PRINT _
```

```bash
chmod +x hello.bas
./hello.bas one two
```

## Shell Passthrough

Lines that don’t parse as BASIC (and aren’t numbered) are executed by `/bin/sh -lc`. Quoted args are supported.

```
echo "hi there"
grep -R "TODO" src
```
