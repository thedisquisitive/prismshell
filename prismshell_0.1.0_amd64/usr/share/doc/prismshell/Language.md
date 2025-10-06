.TH PRISMSHELL-LANGUAGE 7 "2025-10-04" "PrismBASIC" "Miscellanea"
.SH NAME
prismshell-language \- PrismBASIC language reference
.SH SUMMARY
This page documents the PrismBASIC language implemented by prismshell,
including Phase 1 features: arrays, user-defined subroutines, and block control flow.
.SH STATEMENTS
.SS Variables and Arrays
.TP
.B LET name = expr
Assign value to a variable.
.TP
.B DIM name[size]
Declare fixed-size array with
.I size
elements (0-indexed).
.TP
.B DIM name[]
Declare dynamic array that auto-expands when accessed beyond current size.
.TP
.B LET name[index] = expr
Assign value to array element at
.IR index .
.SS Input/Output
.TP
.B PRINT expr
Print value followed by newline.
.TP
.B PRINT expr;
Print value without newline (semicolon suppresses).
.TP
.B PRINT expr, expr
Concatenate and print multiple expressions.
.TP
.B INPUT name
Read a line from stdin into variable.
.SS Conditionals
.TP
.B IF expr THEN line
Single-line jump form: jump to
.I line
if condition is true.
.TP
.B IF expr THEN
Block form: execute following statements if condition is true.
Requires matching
.BR ENDIF .
.TP
.B ELSEIF expr THEN
Alternative condition (only valid inside IF block).
.TP
.B ELSE
Default branch when no prior condition matched.
.TP
.B ENDIF
Close IF block.
.SS Loops
.TP
.B WHILE expr
Execute loop body while condition is true.
Requires matching
.BR WEND .
.TP
.B WEND
End of WHILE loop. Jumps back to matching WHILE to re-check condition.
.SS Jumps and Subroutines
.TP
.B GOTO line
Unconditional jump to line number.
.TP
.B GOSUB line
Call subroutine at line number. Use
.B RETURN
to resume.
.TP
.B RETURN
Return from GOSUB to the line following the call.
.TP
.B SUB name(param1, param2, ...)
Define user subroutine with parameters.
.TP
.B END SUB
End of user subroutine definition.
.TP
.B CALL name(arg1, arg2, ...)
Invoke user-defined SUB or builtin function.
Within a SUB, use
.B LET _ = value
to set return value.
.SS Program Control
.TP
.B END
Terminate program execution.
.TP
.B REM text
Comment to end of line.
.TP
.B ' text
Apostrophe comment (alternative to REM).
.SH EXPRESSIONS
.SS Literals
Numbers
.RB ( 42 ", " 3.14 ", " -1.5 )
and strings
.RB ( \[dq]hello\[dq] ", " \[dq]world\[dq] ).
.SS Variables and Arrays
.TP
.I name
Variable access.
.TP
.I name[index]
Array element access (0-based indexing).
.TP
.B _
Result of last CALL or SUB invocation.
.SS Operators
Arithmetic operators with standard precedence:
.TP
.B ^
Exponentiation (highest precedence).
.TP
.B * /
Multiplication and division.
.TP
.B + -
Addition and subtraction. The
.B +
operator concatenates strings when either operand is a string.
.PP
Comparison operators (return 1 for true, 0 for false):
.TP
.B == !=
Equality and inequality.
.TP
.B < <= > >=
Relational comparisons.
.PP
Unary operators:
.TP
.B -expr
Negation.
.TP
.B +expr
Positive (no-op).
.SS Function Calls
.B FunctionName(arg1, arg2, ...)
can appear in expressions.
The result is stored in the
.B _
variable.
.SH BUILTIN FUNCTIONS
.SS Array Support
.TP
.B LEN(string)
Return length of string.
.TP
.B LEN(\[dq]arrayname\[dq])
Return length of array (pass array name as string).
.SS Environment
.TP
.B Env.Cwd()
Return current working directory.
.TP
.B Env.Args()
Return script arguments from PB_ARGV.
.TP
.B Env.Get(\[dq]VAR\[dq])
Get environment variable value.
.TP
.B Env.Set(\[dq]VAR\[dq], \[dq]VAL\[dq])
Set environment variable.
.TP
.B Env.Exit(code)
Exit process with status code.
.SS Terminal I/O
.TP
.B TTY.ReadLine(prompt)
Read line with prompt string.
.TP
.B TTY.Write(text)
Write text without newline.
.TP
.B TTY.WriteLine(text)
Write text with newline.
.SS Filesystem Operations
.TP
.B FS.Read(path)
Read file contents as string.
.TP
.B FS.Write(path, text)
Write text to file.
.TP
.B FS.Append(path, text)
Append text to file.
.TP
.B FS.Delete(path)
Delete file.
.TP
.B FS.List(path)
List directory contents (newline-separated).
.TP
.B FS.Exists(path)
Return 1 if path exists, 0 otherwise.
.TP
.B FS.Glob(pattern)
Glob pattern match (POSIX only; stubbed on Windows).
.SS Filesystem Management
.TP
.B FS.Cd(path)
Change current directory. Returns 1 on success.
.TP
.B FS.Mkdir(path [, parents])
Create directory.
.I parents
defaults to 1 (create parent directories as needed).
Returns 1 on success.
.TP
.B FS.Rmdir(path)
Remove empty directory. Returns 1 if removed.
.TP
.B FS.RemoveAll(path)
Recursively delete file or directory. Returns 1 on success.
.TP
.B FS.Copy(src, dst [, recursive])
Copy file or directory.
.I recursive
defaults to 0.
Returns 1 on success.
.TP
.B FS.Move(src, dst)
Move or rename file/directory (with cross-device fallback).
Returns 1 on success.
.TP
.B FS.Touch(path)
Create file if missing and update timestamp. Returns 1 on success.
.TP
.B FS.Size(path)
Return file size in bytes (number). Returns 0 on error.
.TP
.B FS.IsDir(path)
Return 1 if path is a directory, 0 otherwise.
.TP
.B FS.MTime(path)
Return last write time in UNIX seconds (number).
.SS Path Manipulation
.TP
.B Path.Join(a, b)
Join two path components.
.TP
.B Path.Dirname(path)
Return parent directory string.
.TP
.B Path.Basename(path)
Return filename component.
.TP
.B Path.Ext(path)
Return file extension including dot (e.g.,
.BR .txt ).
.SS Random Numbers
.TP
.B RNG.Seed([value])
Seed random number generator.
If
.I value
omitted, uses entropy-based seed.
.TP
.B RNG.Int([max])
Return random integer in range [0,
.IR max ].
Default
.I max
is 0x7fffffff.
.TP
.B RNG.Int(min, max)
Return random integer in range [min, max].
.TP
.B RNG.Float()
Return random float in range [0.0, 1.0).
.TP
.B Rand()
Alias for RNG.Float().
.SS Time
.TP
.B Time.Now()
Return current time as string
.RB \[dq] YYYY-MM-DD
.B HH:MM:SS \[dq].
.SS Mod System (Internal)
.TP
.B Mod.Register(\[dq]name\[dq], entryLine)
Register mod command (used by mod authors).
.TP
.B Mod.List()
Return list of registered mods (newline-separated).
.SS Prompt Control
.TP
.B Prompt.SetTemplate(template)
Set prompt template with placeholders.
.TP
.B Prompt.Get()
Get current prompt template.
.SH SPECIAL VARIABLES
.TP
.B _
Result of last CALL or user SUB invocation.
.TP
.B PB_ARGV
Script arguments (JSON-like array string).
.PP
When running as a mod, additional variables are available:
.TP
.B PB_CMD
Command name invoked.
.TP
.B PB_ARGC
Argument count.
.TP
.B PB_ARG1, PB_ARG2, ...
Individual arguments (quote-aware).
.TP
.B PB_ARGS
Space-joined arguments.
.TP
.B PB_PROMPT_TMPL
Prompt template string (when set).
.SH EXAMPLES
.SS Array Usage
.EX
DIM numbers[10]
LET numbers[0] = 42
LET numbers[1] = 100

DIM dynamic[]
LET dynamic[50] = 999  ' Auto-expands

CALL LEN(\[dq]numbers\[dq])
PRINT \[dq]Array size: \[dq]; _
.EE
.SS Block IF Statement
.EX
LET age = 25
IF age < 13 THEN
  PRINT \[dq]Child\[dq]
ELSEIF age < 20 THEN
  PRINT \[dq]Teenager\[dq]
ELSE
  PRINT \[dq]Adult\[dq]
ENDIF
.EE
.SS WHILE Loop
.EX
LET i = 1
WHILE i <= 5
  PRINT \[dq]Count: \[dq]; i
  LET i = i + 1
WEND
.EE
.SS User Subroutine
.EX
SUB Factorial(n)
  IF n <= 1 THEN
    LET _ = 1
  ELSE
    CALL Factorial(n - 1)
    LET _ = n * _
  ENDIF
END SUB

CALL Factorial(5)
PRINT \[dq]5! = \[dq]; _
.EE
.SH NOTES
.IP \(bu 3
Arrays are 0-indexed.
.IP \(bu 3
Dynamic arrays auto-expand when accessed beyond current size.
.IP \(bu 3
User SUBs can be recursive.
.IP \(bu 3
SUBs inherit parent variables but modifications are local.
.IP \(bu 3
Return values from SUBs use the
.B _
variable.
.IP \(bu 3
Block control flow (IF/WHILE) supports nesting.
.SH SEE ALSO
.BR prismshell (1),
.BR prismshell-mods (7),
.BR prismshell-prompt (7)