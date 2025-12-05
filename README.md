JS Syntax Parser (C, re2c + bison) with ASI

- Purpose: Check whether input JavaScript is syntactically valid; implements Automatic Semicolon Insertion (ASI) for omitted semicolons.
- Tools: re2c (lexer), bison (parser), gcc (C11).
- Standard: Guided by ECMA-262; implemented a practical subset suitable for coursework. Extend as needed.

Build
- deps: re2c, bison, gcc
- make

Usage
- ./bin/jsparser file.js
  - Exit code 0: OK
  - Exit code 1: Syntax error (prints line:col and message)
  - Exit code 2: IO or internal error

Design
- Lexer (re2c):
  - Tokens for keywords/operators/literals/identifiers.
  - Tracks line/column; tracks LineTerminator presence between tokens (for ASI).
  - Exposes `drv->has_line_terminator = 1` when a line break occurred while skipping whitespace/comments before the next token.
- Parser (bison):
  - Covers: program, block, var/let/const declarations, function declarations, if/while/do-while/for, break/continue/return/throw, try/catch/finally (catch binding optional), switch/case/default, labelled statements, debugger, expression statements, assignment, logical/relational/arithmetic/bitwise, unary/postfix, member access/call, array/object literals with trailing commas, `for-in`/`for-of`.
  - ASI:
    * `semi_opt` accepts `;` or inserts automatically when:
      - A LineTerminator occurred before the next token, or
      - Next token is `}` or EOF.
    * Restricted productions (`return/throw/break/continue` and postfix `++/--`) enforce "no LineTerminator here".
- Output:
  - On success: prints "OK".
  - On error: prints "Syntax error at line X, col Y: <message>".

Testing
- make test
  - test/js/valid: positive samples (many ASI edges included).
  - test/js/invalid: negative samples.
  - test/vb: put VB .vb files; they should be rejected.

Corpus
- JS dataset: make corpus-js DATASET_JS=/path/to/js/dataset (e.g., ZZN0508/JavaScript_Datasetst)
- VB dataset: make corpus-vb DATASET_VB=/path/to/vb_examples (e.g., rubenhorras/vb_examples)

Notes
- Identifiers: ASCII [_$A-Za-z][_$A-Za-z0-9]*; re2c can be extended to Unicode ID_Start/ID_Continue by adding tables.
- Not implemented: modules (import/export), class/extends, template strings, regex literals (need context-sensitive lexing), optional chaining, nullish coalescing, destructuring, arrow functions, yield/await, etc. These can be incrementally added.
