# Tether Language Specification (Complete Draft)

This document provides a comprehensive reference specification for the Tether programming language. The specification includes lexical analysis, syntax grammar, semantic rules, and implementation guidance for building a complete Tether compiler/interpreter.

## 1. Language Overview

Tether is a systems programming language designed for high-performance computing with built-in support for parallelization and distributed computing. The language emphasizes:

- Static typing with type inference
- Memory safety without garbage collection
- First-class support for parallel and distributed computing
- C-like syntax with modern language features
- Efficient compilation to native code

## 2. Notation and Conventions

### 2.1 Grammar Notation
- Terminal symbols are enclosed in single quotes: `'fn'`, `'='`
- Non-terminals are enclosed in angle brackets: `<expression>`, `<statement>`
- Square brackets `[...]` denote optional elements
- Curly braces `{...}` denote repetition (zero or more)
- Parentheses `(...)` group elements
- Vertical bar `|` denotes alternatives
- Ellipsis `...` indicates omitted repetitive content

### 2.2 Character Classes
- `<letter>` ::= `'A'..'Z'` | `'a'..'z'` | `'_'`
- `<digit>` ::= `'0'..'9'`
- `<hex_digit>` ::= `<digit>` | `'A'..'F'` | `'a'..'f'`
- `<binary_digit>` ::= `'0'` | `'1'`
- `<octal_digit>` ::= `'0'..'7'`

## 3. Lexical Analysis

### 3.1 Input Format
- Source files are UTF-8 encoded text
- File extension: `.tx` or `.th`
- Line endings: `\n`, `\r\n`, or `\r`

### 3.2 Whitespace and Comments

#### Whitespace
```ebnf
<whitespace> ::= ' ' | '\t' | '\r' | '\n'
```

#### Comments
```ebnf
<line_comment> ::= '//' { <any_char_except_newline> } <newline>
<block_comment> ::= '/*' { <any_char> } '*/'
```

Block comments may be nested. Comments are treated as whitespace.

### 3.3 Identifiers and Keywords

#### Identifiers
```ebnf
<identifier> ::= <letter> { <letter> | <digit> }
```

Identifiers are case-sensitive and may not begin with a digit.

#### Keywords
```
break     continue  else      false     fn        for
if        import    in        let       return    true
while     const     mut       struct    enum      match
pub       priv      mod       use       self      super
```

Keywords are reserved and cannot be used as identifiers.

### 3.4 Literals

#### Integer Literals
```ebnf
<integer_literal> ::= <decimal_literal> | <hex_literal> | <binary_literal> | <octal_literal>
<decimal_literal> ::= <digit> { <digit> | '_' }
<hex_literal> ::= '0x' <hex_digit> { <hex_digit> | '_' }
<binary_literal> ::= '0b' <binary_digit> { <binary_digit> | '_' }
<octal_literal> ::= '0o' <octal_digit> { <octal_digit> | '_' }
```

#### Floating Point Literals
```ebnf
<float_literal> ::= <decimal_literal> '.' <decimal_literal> [ <exponent> ]
                  | <decimal_literal> <exponent>
<exponent> ::= ('e' | 'E') ['+' | '-'] <decimal_literal>
```

#### String Literals
```ebnf
<string_literal> ::= '"' { <string_char> } '"'
<string_char> ::= <printable_char> | <escape_sequence>
<escape_sequence> ::= '\' ('n' | 't' | 'r' | '\' | '"' | '0' | 'u' <hex_digit>{4} | 'U' <hex_digit>{8})
```

#### Character Literals
```ebnf
<char_literal> ::= "'" (<printable_char> | <escape_sequence>) "'"
```

#### Boolean Literals
```ebnf
<bool_literal> ::= 'true' | 'false'
```

### 3.5 Operators and Punctuation

#### Arithmetic Operators
```
+  -  *  /  %  **
```

#### Comparison Operators
```
==  !=  <  >  <=  >=
```

#### Logical Operators
```
&&  ||  !
```

#### Bitwise Operators
```
&  |  ^  <<  >>  ~
```

#### Assignment Operators
```
=  +=  -=  *=  /=  %=  &=  |=  ^=  <<=  >>=
```

#### Other Punctuation
```
;  ,  .  ::  ->  =>  ?  :
(  )  {  }  [  ]
```

### 3.6 Decorators
```ebnf
<decorator> ::= '@' <identifier> [ '(' <decorator_args> ')' ]
<decorator_args> ::= <expression> { ',' <expression> }
```

### 3.7 Type Names
```ebnf
<builtin_type> ::= 'i8' | 'i16' | 'i32' | 'i64' | 'isize'
                 | 'u8' | 'u16' | 'u32' | 'u64' | 'usize'
                 | 'f32' | 'f64'
                 | 'bool' | 'char' | 'str'
```

## 4. Syntax Grammar

### 4.1 Program Structure
```ebnf
<program> ::= { <item> }

<item> ::= <function_item>
         | <struct_item>
         | <enum_item>
         | <const_item>
         | <use_item>
         | <mod_item>
         | <decorated_item>

<decorated_item> ::= { <decorator> } <item>
```

### 4.2 Modules and Imports
```ebnf
<use_item> ::= 'use' <module_path> ';'
<mod_item> ::= 'mod' <identifier> ( ';' | '{' { <item> } '}' )
<module_path> ::= <identifier> { '::' <identifier> }
```

### 4.3 Function Definitions
```ebnf
<function_item> ::= <visibility> 'fn' <identifier> <generic_params> '(' <parameter_list> ')' <return_type> <block_expr>

<visibility> ::= [ 'pub' | 'priv' ]
<generic_params> ::= [ '<' <generic_param_list> '>' ]
<generic_param_list> ::= <identifier> { ',' <identifier> }
<parameter_list> ::= [ <parameter> { ',' <parameter> } ]
<parameter> ::= <pattern> ':' <type>
<return_type> ::= [ '->' <type> ]
```

### 4.4 Type Definitions
```ebnf
<struct_item> ::= <visibility> 'struct' <identifier> <generic_params> '{' <struct_fields> '}'
<struct_fields> ::= [ <struct_field> { ',' <struct_field> } [ ',' ] ]
<struct_field> ::= <visibility> <identifier> ':' <type>

<enum_item> ::= <visibility> 'enum' <identifier> <generic_params> '{' <enum_variants> '}'
<enum_variants> ::= [ <enum_variant> { ',' <enum_variant> } [ ',' ] ]
<enum_variant> ::= <identifier> [ '(' <type_list> ')' | '{' <struct_fields> '}' ]
```

### 4.5 Constants
```ebnf
<const_item> ::= <visibility> 'const' <identifier> ':' <type> '=' <expression> ';'
```

### 4.6 Types
```ebnf
<type> ::= <path_type>
         | <array_type>
         | <slice_type>
         | <reference_type>
         | <function_type>
         | <tuple_type>
         | '(' <type> ')'

<path_type> ::= <type_path>
<array_type> ::= '[' <type> ';' <expression> ']'
<slice_type> ::= '[' <type> ']'
<reference_type> ::= '&' [ 'mut' ] <type>
<function_type> ::= 'fn' '(' <type_list> ')' [ '->' <type> ]
<tuple_type> ::= '(' [ <type_list> ] ')'

<type_list> ::= <type> { ',' <type> }
<type_path> ::= <identifier> { '::' <identifier> } [ '<' <type_list> '>' ]
```

### 4.7 Patterns
```ebnf
<pattern> ::= <identifier_pattern>
            | <literal_pattern>
            | <tuple_pattern>
            | <struct_pattern>
            | <wildcard_pattern>

<identifier_pattern> ::= [ 'mut' ] <identifier>
<literal_pattern> ::= <literal>
<tuple_pattern> ::= '(' [ <pattern_list> ] ')'
<struct_pattern> ::= <type_path> '{' <field_patterns> '}'
<wildcard_pattern> ::= '_'

<pattern_list> ::= <pattern> { ',' <pattern> }
<field_patterns> ::= [ <field_pattern> { ',' <field_pattern> } [ ',' '..' ] ]
<field_pattern> ::= <identifier> [ ':' <pattern> ]
```

### 4.8 Statements
```ebnf
<statement> ::= <expression_stmt>
              | <let_stmt>
              | <item>

<expression_stmt> ::= <expression> ';'
<let_stmt> ::= 'let' <pattern> [ ':' <type> ] [ '=' <expression> ] ';'
```

### 4.9 Expressions
```ebnf
<expression> ::= <assignment_expr>

<assignment_expr> ::= <logical_or_expr> [ <assignment_op> <assignment_expr> ]
<assignment_op> ::= '=' | '+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^=' | '<<=' | '>>='

<logical_or_expr> ::= <logical_and_expr> { '||' <logical_and_expr> }
<logical_and_expr> ::= <equality_expr> { '&&' <equality_expr> }

<equality_expr> ::= <relational_expr> { ('==' | '!=') <relational_expr> }
<relational_expr> ::= <bitwise_or_expr> { ('<' | '>' | '<=' | '>=') <bitwise_or_expr> }

<bitwise_or_expr> ::= <bitwise_xor_expr> { '|' <bitwise_xor_expr> }
<bitwise_xor_expr> ::= <bitwise_and_expr> { '^' <bitwise_and_expr> }
<bitwise_and_expr> ::= <shift_expr> { '&' <shift_expr> }

<shift_expr> ::= <additive_expr> { ('<<' | '>>') <additive_expr> }
<additive_expr> ::= <multiplicative_expr> { ('+' | '-') <multiplicative_expr> }
<multiplicative_expr> ::= <power_expr> { ('*' | '/' | '%') <power_expr> }
<power_expr> ::= <unary_expr> { '**' <unary_expr> }

<unary_expr> ::= <postfix_expr>
               | <unary_op> <unary_expr>
<unary_op> ::= '!' | '-' | '+' | '~' | '&' | '&mut'

<postfix_expr> ::= <primary_expr> { <postfix_op> }
<postfix_op> ::= '[' <expression> ']'
               | '.' <identifier>
               | '(' <argument_list> ')'

<primary_expr> ::= <literal>
                 | <identifier>
                 | <path_expr>
                 | <array_expr>
                 | <tuple_expr>
                 | <struct_expr>
                 | <block_expr>
                 | <if_expr>
                 | <match_expr>
                 | <for_expr>
                 | <while_expr>
                 | <loop_expr>
                 | '(' <expression> ')'

<path_expr> ::= <type_path>
<array_expr> ::= '[' [ <expression_list> ] ']'
<tuple_expr> ::= '(' [ <expression_list> ] ')'
<struct_expr> ::= <type_path> '{' <field_inits> '}'

<expression_list> ::= <expression> { ',' <expression> }
<argument_list> ::= [ <expression_list> ]
<field_inits> ::= [ <field_init> { ',' <field_init> } [ ',' '..' <expression> ] ]
<field_init> ::= <identifier> [ ':' <expression> ]
```

### 4.10 Control Flow Expressions
```ebnf
<block_expr> ::= '{' { <statement> } [ <expression> ] '}'

<if_expr> ::= 'if' <expression> <block_expr> [ 'else' ( <block_expr> | <if_expr> ) ]

<match_expr> ::= 'match' <expression> '{' <match_arms> '}'
<match_arms> ::= { <match_arm> }
<match_arm> ::= <pattern> [ 'if' <expression> ] '=>' ( <expression> | <block_expr> ) ','

<for_expr> ::= 'for' <pattern> 'in' <expression> <block_expr>
<while_expr> ::= 'while' <expression> <block_expr>
<loop_expr> ::= 'loop' <block_expr>

<break_expr> ::= 'break' [ <expression> ]
<continue_expr> ::= 'continue'
<return_expr> ::= 'return' [ <expression> ]
```

## 5. Decorator System

### 5.1 Built-in Decorators

#### @parallel
Indicates that a function or loop should be executed in parallel across available CPU cores.

```tx
@parallel
fn parallel_sum(data: [i32]) -> i32 {
    let mut total = 0;
    for item in data {
        total += item;
    }
    total
}
```

#### @vectorize
Requests SIMD vectorization for mathematical operations.

```tx
@vectorize
fn vector_add(a: [f32], b: [f32]) -> [f32] {
    let mut result = [0.0; a.len()];
    for i in 0..a.len() {
        result[i] = a[i] + b[i];
    }
    result
}
```

#### @inline
Suggests function inlining for performance.

```tx
@inline
fn fast_square(x: f64) -> f64 {
    x * x
}
```

#### @extern
Marks a function or global as having external (C-compatible) linkage and requests that the compiler emit a stable, unmangled symbol name for FFI/ABI usage.

```tx
@extern
pub fn c_compatible_function(x: i32) -> i32 {
    x * 2
}
```

### 5.2 Decorator Semantics
- Decorators are compile-time attributes that provide hints to the compiler
- Multiple decorators can be applied to a single item
- Unknown decorators generate warnings but do not cause compilation failure
- Decorators can take parameters for configuration

## 6. Memory Model

### 6.1 Ownership
- Each value has a single owner
- Values are moved by default
- References allow borrowing without transferring ownership

### 6.2 Borrowing Rules
- Any number of immutable references OR one mutable reference
- References must not outlive the data they point to
- No null references

### 6.3 Lifetimes
- Compile-time construct ensuring memory safety
- Automatic lifetime inference in most cases
- Explicit lifetime annotations when needed

## 7. Type System

### 7.1 Primitive Types

#### Integer Types
- Signed: `i8`, `i16`, `i32`, `i64`, `isize`
- Unsigned: `u8`, `u16`, `u32`, `u64`, `usize`
- Default integer type: `i32`

#### Floating Point Types
- `f32`: 32-bit floating point
- `f64`: 64-bit floating point
- Default float type: `f64`

#### Other Primitives
- `bool`: boolean values (`true`, `false`)
- `char`: Unicode scalar values
- `str`: string slices (UTF-8)

### 7.2 Compound Types

#### Arrays
- Fixed-size sequences: `[T; N]`
- All elements must be the same type
- Size known at compile time

#### Slices
- Dynamic views into sequences: `[T]`
- Runtime-sized but not growable
- Usually seen as references: `&[T]`

#### Tuples
- Heterogeneous sequences: `(T1, T2, ...)`
- Fixed size, each element can have different type
- Accessed by position

#### Structs
- Custom types grouping related data
- Named fields with types

#### Enums
- Types with multiple variants
- Can carry data in variants
- Pattern matching for safe access

### 7.3 Generics
- Type parameters for functions and structs
- Compile-time monomorphization
- Trait bounds for constraining type parameters

## 8. Standard Library Overview

### 8.1 Core Modules

#### `std::mem`
Memory management utilities

#### `std::collections`
Data structures (Vec, HashMap, etc.)

#### `std::io`
Input/output operations

#### `std::thread`
Threading and parallelism

#### `std::math`
Mathematical functions and constants

#### `std::string`
String manipulation

## 9. Example Programs

### 9.1 Hello World
```tx
fn main() {
    println("Hello, Tether!");
}
```

### 9.2 Fibonacci Sequence
```tx
fn fibonacci(n: u32) -> u64 {
    if n <= 1 {
        n as u64
    } else {
        fibonacci(n - 1) + fibonacci(n - 2)
    }
}

fn main() {
    for i in 0..10 {
        println("fib({}) = {}", i, fibonacci(i));
    }
}
```

### 9.3 Parallel Processing
```tx
use std::thread;

@parallel
fn parallel_map<T, U>(data: [T], f: fn(T) -> U) -> [U] {
    let mut results = Vec::with_capacity(data.len());
    
    for item in data {
        results.push(f(item));
    }
    
    results
}

fn main() {
    let numbers = [1, 2, 3, 4, 5];
    let squared = parallel_map(numbers, |x| x * x);
    println("Squared: {:?}", squared);
}
```

### 9.4 Struct and Methods
```tx
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    fn new(x: f64, y: f64) -> Point {
        Point { x, y }
    }
    
    fn distance(&self, other: &Point) -> f64 {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }
}

fn main() {
    let p1 = Point::new(0.0, 0.0);
    let p2 = Point::new(3.0, 4.0);
    println("Distance: {}", p1.distance(&p2));
}
```

## 10. Error Handling

### 10.1 Compile-time Errors
- Syntax errors: malformed code
- Type errors: type mismatches
- Borrow checker errors: ownership violations
- Name resolution errors: undefined identifiers

### 10.2 Runtime Errors
- Panic: unrecoverable errors
- Result type: recoverable errors
- Option type: handling null-like values

### 10.3 Error Recovery
- Parser recovery strategies for better error messages
- Multiple error reporting in single compilation pass

## 11. Implementation Notes

### 11.1 Lexer Implementation
- Longest match tokenization
- Unicode support for identifiers and strings
- Efficient keyword recognition using perfect hashing
- Proper handling of nested block comments

### 11.2 Parser Implementation
- Recursive descent parser with precedence climbing
- Error recovery using panic mode and synchronization tokens
- AST construction with source location information
- Support for incremental parsing for IDE integration

### 11.3 Semantic Analysis
- Symbol table management with scope tracking
- Type checking with inference and unification
- Borrow checker implementation
- Decorator processing and code generation hints

### 11.4 Code Generation
- LLVM backend for native code generation
- Optimization pipeline integration
- Debugging information generation
- Target-specific optimizations

## 12. Future Extensions

### 12.1 Planned Features
- Async/await support
- More sophisticated generic constraints
- Compile-time evaluation
- Foreign function interface improvements

### 12.2 Research Areas
- Distributed computing primitives
- Automatic parallelization
- Domain-specific optimizations
- Interactive development tools

---

This specification serves as the foundation for implementing the Tether programming language. It provides sufficient detail for building a complete compiler while leaving room for future enhancements and optimizations.