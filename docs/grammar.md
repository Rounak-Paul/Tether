# Tether — Mini Grammar & Lexical Notes

This file captures a minimal grammar sketch and lexical/tokenization notes for the Phase 1 lexer and parser. It's intentionally compact — enough to start implementing a tokenizer and parser in C.

## Lexical notes

-- Use semicolons (`;`) to terminate statements and braces (`{` / `}`) to define blocks.
- UTF-8 source. Newlines separate logical lines unless within parentheses or with trailing `\`.
- Comments: `# single-line comment` and block comments `/* ... */` (optional for parser; can be skipped by lexer).
- Identifiers: ASCII letters, digits, and `_`, not starting with a digit: `[A-Za-z_][A-Za-z0-9_]*`.
- Numbers: integer and floating forms (decimal). Suffixes optional: `i32`, `f32` in type annotations only.
- Strings: double-quoted `"..."` with standard escapes.

## Significant tokens

- NEWLINE (optional), SEMI (';'), LBRACE '{', RBRACE '}'
- DEF, RETURN, IF, ELSE, FOR, IN, PARALLEL, IMPORT, FROM
- AT (@) for decorators: `@kernel`, `@gpu`
- COLON(:), COMMA(,), LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE
- ARROW (->) for return type annotations (optional)
- TYPE_ANNOT (':' + type)
- ASSIGN (=), PLUS(+), MINUS(-), STAR(*), SLASH(/), PERCENT(%), EQ(==), NE(!=), LT(<), GT(>) etc.

## High-level parsing targets (informal BNF)


program := statement*

statement := simple_stmt SEMI | compound_stmt

simple_stmt := expression_stmt | assign_stmt | return_stmt | import_stmt

expression_stmt := expression
assign_stmt := identifier ':' type '=' expression
return_stmt := 'return' expression?
import_stmt := 'import' identifier ('.' identifier)*

compound_stmt := func_def | if_stmt | for_stmt | parallel_for_stmt | block

func_def := decorators? 'def' identifier '(' param_list? ')' return_type? block
decorators := decorator+
decorator := '@' identifier ('(' arg_list? ')')?
param_list := (param (',' param)*)
param := identifier ':' type ( '=' expression)?
return_type := '->' type

if_stmt := 'if' '(' expression ')' block ( 'else' block)?

for_stmt := 'for' '(' for_init ';' for_cond ';' for_post ')' block
for_init := assign_stmt | expression_stmt | /* empty */
for_cond := expression | /* empty */
for_post := expression | assign_stmt | /* empty */

parallel_for_stmt := 'parallel' 'for' '(' for_init ';' for_cond ';' for_post ')' block

block := '{' statement* '}'

expression := binary_expr | unary_expr | atom

binary_expr := expression operator expression
unary_expr := ('-' | '!' ) expression
atom := literal | identifier | call | array_literal | '(' expression ')'
call := identifier '(' arg_list? ')'
arg_list := expression (',' expression)*

type := simple_type | generic_type
cast_expr := type '(' expression ')'  // explicit cast, e.g. f32(x)
simple_type := 'i8' | 'i16' | 'i32' | 'i64' | 'u8' | 'u16' | 'u32' | 'u64' | 'f32' | 'f64' | 'bool' | 'string' | identifier
generic_type := identifier '[' type (',' type)* ']'

## Operator precedence (high→low)

1. Parentheses
2. Indexing / attribute access
3. Unary (+, - , not)
4. Multiplicative (*, /, %)
5. Additive (+, -)
6. Comparisons (<, >, <=, >=)
7. Equality (==, !=)
8. Logical and
9. Logical or

## Special runtime hooks (for parser/semantics)

- `@kernel` decorator: marks a function as a device kernel. Must have simple parameter types.
- `@gpu` decorator: suggests GPU placement; runtime may ignore if unavailable.
- `global_idx()` / `local_idx()` are built-in intrinsics available inside kernels.

## Notes for lexer implementer

- Emit INDENT/DEDENT like Python: track leading spaces (tabs discouraged). Use a simple stack of indent sizes.
- Treat decorators as separate tokens preceding the function definition.
- For initial bootstrap, keep grammar LL(1)/recursive-descent friendly: avoid complicated context-sensitive constructs.

## Minimal parsing goals for Phase 1

1. Parse function definitions, calls, assignments, basic expressions, conditionals, and for-loops.
2. Recognize `parallel for` as a distinct compound statement.
3. Recognize decorators and attach them to following function defs.
4. Parse type annotations (optional) on params and return types.

---

This is a living sketch; adapt as the implementation reveals parsing edge cases.
