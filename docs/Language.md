# PrismBASIC (MVP) Language

## Statements
- `LET name = expr`
- `PRINT expr` — append `;` to omit newline: `PRINT expr;`
- `INPUT name`
- `IF expr THEN <line>` — jump-to-line form (no `ENDIF` blocks yet)
- `GOTO <line>`
- `GOSUB <line>` / `RETURN`
- `CALL QName(args...)`
- `END`
- `REM ...` — comment to end of line

## Expressions
- Literals: numbers, strings
- Variables
- Function-like calls in expressions: `Foo.Bar(...)`
- Binary ops: `+ - * /` (standard precedence); `+` concatenates strings
- Comparisons: `== != < <= > >=` (return `1` or `0`)
- Unary `-` supported: `LET X = -1`

## Special Variables
- `_` — result of the last `CALL`
- `PB_ARGV` — script argv (JSON-ish array string in MVP)
- In mods:
  - `PB_CMD`, `PB_ARGC`
  - `PB_ARG1..PB_ARGN`, `PB_ARGS`, `PB_ARGV`

## Builtin CALLs
 (MVP)
- `Env.Cwd()` → string
- `Env.Args()` → argv (JSON-ish string via `PB_ARGV`)
- `Env.Get("VAR")` / `Env.Set("VAR","VAL")`
- `Env.Exit(code)`
- `TTY.ReadLine(prompt)` / `TTY.Write(text)` / `TTY.WriteLine(text)`

**Filesystem**  
- `FS.Read(path)` / `FS.Write(path,text)` / `FS.Append(path,text)`  
- `FS.Delete(path)` / `FS.List(path[, "*suffix"])` / `FS.Exists(path)` / `FS.Glob(pattern)` *(POSIX; stubbed on Windows)*  
- `FS.Cd(path)` → `1` on success  
- `FS.Mkdir(path[, parents=1])` → `1` if created  
- `FS.Rmdir(path)` → `1` if removed  
- `FS.RemoveAll(path)` → recursive delete; `1` on success  
- `FS.Copy(src, dst[, recursive=0])` → `1` on success  
- `FS.Move(src, dst)` → `1` on success (rename, with cross-device copy fallback)  
- `FS.Touch(path)` → `1` (create if missing, update mtime)  
- `FS.Size(path)` → size in bytes (number; `0` on error)  
- `FS.IsDir(path)` → `1` if directory else `0`  
- `FS.MTime(path)` → last write time (UNIX seconds, number)

**Path helpers**  
- `Path.Join(a,b)` → joined path  
- `Path.Dirname(path)` → parent directory  
- `Path.Basename(path)` → filename  
- `Path.Ext(path)` → extension including dot (e.g. `.txt`)
