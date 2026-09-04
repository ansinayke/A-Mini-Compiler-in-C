# Mini Compiler in C

[![Language: C](https://img.shields.io/badge/Language-C-A8B9CC?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Compiler: Clang/GCC](https://img.shields.io/badge/Compiler-Clang%20%2F%20GCC-555555?logo=llvm&logoColor=white)](https://clang.llvm.org/)
[![Parsing: Recursive Descent](https://img.shields.io/badge/Parsing-Recursive%20Descent-2E7D32)](CFG.txt)
[![Status: Educational Project](https://img.shields.io/badge/Status-Educational%20Project-1565C0)](compiler.c)

A compact compiler front end and interpreter-style evaluator written in C. The project accepts a small C-like language, converts source text into tokens, validates it against a context-free grammar, evaluates arithmetic expressions, stores variables in a symbol table, and reports syntax or semantic errors with line information.

This project was built to make the core stages of language processing tangible: source code is not treated as a magical string, but as a stream that must be recognized, structured, validated, and evaluated.

## Highlights

- Hand-written lexical analyzer for keywords, identifiers, numeric literals, operators, parentheses, and semicolons.
- Recursive-descent parser organized around `expression`, `term`, and `factor` functions.
- Correct arithmetic precedence: multiplication and division bind more tightly than addition and subtraction.
- Unary negative values and nested parenthesized expressions.
- `int` and `float` declarations using a shared numeric representation.
- Symbol table with declaration checks, variable lookup, redeclaration detection, and capacity protection.
- Immediate evaluation of declarations and `print(...)` statements.
- Defensive error handling for invalid characters, malformed numbers, undeclared variables, missing syntax, division by zero, and invalid statements.
- A separate grammar reference in [CFG.txt](CFG.txt), sample source in [input.txt](input.txt), and an invalid-input example in [error.txt](error.txt).

## Supported Language

```text
program     -> statement*
statement   -> declaration | print
declaration -> (int | float) ID '=' expression ';'
print       -> print '(' expression ')' ';'
expression  -> term (('+' | '-') term)*
term        -> factor (('*' | '/') factor)*
factor      -> '-' factor | NUMBER | ID | '(' expression ')'
```

Example program:

```c
int y = 5;
int x = 115;
int z = x + y;
print(z);
```

Output:

```text
120
```

## Build and Run

Compile with a C compiler (GNU compiler used here):

```bash
gcc compiler.c -o parser
```

GCC can be used with the same command by replacing `clang` with `gcc`.

Run a source file:

```bash
./parser input.txt
```

The executable reads the source file passed as its first argument and prints evaluated results or a line-aware error message.

## Implementation Guidelines

The implementation follows a deliberately small, explicit architecture:

1. **Tokenization:** `getNextToken()` reads characters from the source file and produces typed tokens. Whitespace is skipped while line numbers are tracked.
2. **Parsing:** `parseProgram()` dispatches statements. `parseExpr()`, `parseTerm()`, and `parseFactor()` mirror the grammar and encode operator precedence through call structure.
3. **Semantic state:** declarations are inserted into the symbol table only after their complete syntax and initializer expression have been accepted.
4. **Evaluation:** expressions are evaluated as they are parsed, which keeps the project focused and avoids introducing an intermediate representation before it is needed.
5. **Diagnostics:** `match()` and `syntaxError()` centralize expected-token checks and consistent failure reporting.

For future extensions, the cleanest progression would be to separate the lexer, parser, semantic analysis, and evaluator into modules, introduce token positions, and add an abstract syntax tree before supporting richer types or code generation.

## Technical Decisions

- **C:** Chosen for direct control over file I/O, memory layout, data structures, and the mechanics of a compiler pipeline. It also makes the cost of each abstraction visible.
- **Recursive descent:** Chosen because the grammar is small, readable, and naturally maps to one parsing function per grammar rule. This makes precedence and error locations easy to reason about.
- **A symbol table:** Required to connect identifiers in later expressions to values established by declarations, while enabling useful semantic errors for undeclared or redeclared variables.
- **`double` values:** A pragmatic shared representation that lets `int` and `float` declarations participate in the same arithmetic evaluator without duplicating expression logic.
- **Immediate evaluation:** Keeps the implementation compact and demonstrates the full tokenize-parse-evaluate path. A production compiler would typically preserve an intermediate representation or generate target code instead.

## What This Project Demonstrates

This project demonstrates more than arithmetic. It shows the ability to translate a formal grammar into working code, maintain state across statements, preserve language semantics such as precedence, design failure paths, and validate behavior with both successful and intentionally invalid programs.

The most important learning outcome was becoming more comfortable turning an abstract specification into a sequence of concrete, testable stages. It strengthened my understanding of parsing, data representation, interfaces between components, and why good diagnostics are part of language design rather than an afterthought.

## Project Files

| File | Purpose |
| --- | --- |
| `compiler.c` | Lexer, parser, symbol table, evaluator, diagnostics, and program entry point |
| `CFG.txt` | Context-free grammar for the supported language |
| `input.txt` | Valid sample program |
| `error.txt` | Invalid sample program for diagnostic behavior |
| `Report.pages` | Project report |

## Scope and Next Steps

This is an educational mini compiler, intentionally focused on a small language. It does not currently generate machine code, support functions or control flow, perform static type checking, or handle comments and libraries. Natural next steps include an AST, richer type rules, better token metadata, automated tests, and a bytecode or assembly backend.

## Author Note

I built this project to move beyond using programming languages and understand how one is processed underneath. The result is a small but complete language-processing exercise with explicit tradeoffs, a readable grammar, and failure cases that make the implementation easy to inspect.