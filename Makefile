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

# directory for binaries
BIN_DIR := bin

all: $(BIN_DIR) $(BIN_DIR)/tether-node $(BIN_DIR)/typecheck-runner $(BIN_DIR)/parser-runner $(BIN_DIR)/tether

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/parser-runner: $(PR_OBJ) src/parser.o src/token.o src/ast.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/parser-runner $(PR_OBJ) src/parser.o src/token.o src/ast.o $(LDFLAGS)

$(BIN_DIR)/tether: $(TR_OBJ) $(IR_OBJ) src/parser.o src/token.o src/ast.o src/typechecker.o | $(BIN_DIR)
	# tx_runner is the main script runner; produce binary named `tether`
	$(CC) $(CFLAGS) -o $(BIN_DIR)/tether $(TR_OBJ) $(IR_OBJ) src/parser.o src/token.o src/ast.o src/typechecker.o $(LDFLAGS)

$(BIN_DIR)/tether-node: $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/tether-node $(OBJ) $(LDFLAGS)

$(BIN_DIR)/typecheck-runner: $(TC_OBJ) src/typechecker.o src/parser.o src/token.o src/ast.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/typecheck-runner $(TC_OBJ) src/typechecker.o src/parser.o src/token.o src/ast.o $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TC_OBJ) $(BIN_DIR)/tether-node $(BIN_DIR)/typecheck-runner $(BIN_DIR)/parser-runner $(BIN_DIR)/tether

.PHONY: all clean
