- means not implemented yet, + means done, ? means not sure, = means in dev
# LONGTERM:
- add environment arrays, arbirtrary floating point precision https://gitlab.inria.fr/mpfr/mpfr
= add shell options [help, version, rc, rc-file]
- invert boolean function returns, false means success, true means error.

### Extra features:
- actual error handling:
  1. script-mode: `file:linenumber:column: Error message`
  2. interactive-mode: `linenumber:column: Error message` 
? simplified keywords: [declare, if, else, else if, while, do while, for, case,
  export, unset, env,]

### PROMPT
- implement \\ escaping
? make like bash.
### ENV
- n/a
### LEXER
- n/a
### PARSER
- copy oldstuff/prototypes/cmd\_search.c but make it hash!
### EXEC
- make function that executes commands, list of commands, or assignment.
? create an AStree.

