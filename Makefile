CC = gcc
CFLAGS = -std=c11 -O2 -Iinclude -Wall -Wextra
LDFLAGS =

SRC = src/main.c src/lexer.c src/parser.c src/typechecker.c src/runtime.c src/net.c src/token.c src/ast.c
OBJ = $(SRC:.c=.o)

TC_SRC = src/typecheck_runner.c
TC_OBJ = $(TC_SRC:.c=.o)

PR_SRC = src/parser_runner.c
PR_OBJ = $(PR_SRC:.c=.o)

IR_SRC = src/interpreter.c
IR_OBJ = $(IR_SRC:.c=.o)

TR_SRC = src/tx_runner.c
TR_OBJ = $(TR_SRC:.c=.o)

all: tether-node typecheck-runner
 
parser-runner: $(PR_OBJ) src/parser.o src/token.o src/ast.o
	$(CC) $(CFLAGS) -o parser-runner $(PR_OBJ) src/parser.o src/token.o src/ast.o $(LDFLAGS)

tx-runner: $(TR_OBJ) $(IR_OBJ) src/parser.o src/token.o src/ast.o src/typechecker.o
	$(CC) $(CFLAGS) -o tx-runner $(TR_OBJ) $(IR_OBJ) src/parser.o src/token.o src/ast.o src/typechecker.o $(LDFLAGS)

tether-node: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

typecheck-runner: $(TC_OBJ) src/typechecker.o src/parser.o src/token.o src/ast.o
	$(CC) $(CFLAGS) -o typecheck-runner $(TC_OBJ) src/typechecker.o src/parser.o src/token.o src/ast.o $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TC_OBJ) tether-node typecheck-runner

.PHONY: all clean
