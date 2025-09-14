#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/token.h"
#include "../include/ast.h"

// A tiny runner that calls parser_parse_ast on an input file and prints a brief summary.
// This avoids invoking the typechecker; it's aimed at parser unit tests only.

extern AST *parser_parse_ast(const char *src);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: parser-runner <file.tx>\n");
        return 2;
    }
    const char *path = argv[1];
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); return 2; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz+1);
    if (!buf) { fclose(f); return 2; }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);

    AST *ast = parser_parse_ast(buf);
    if (!ast) {
        printf("PARSE_ERR\n");
        free(buf);
        return 1;
    }
    printf("PARSE_OK funcs=%d vars=%d\n", ast->func_count, ast->var_count);
    for (int i = 0; i < ast->func_count; ++i) {
        FuncDef *fd = &ast->funcs[i];
        printf("func %s params=%d ret=%s stmts=%d\n", fd->name, fd->param_count, fd->ret_type ? fd->ret_type : "<none>", fd->stmt_count);
    }

    // free AST
    ast_free(ast);
    free(buf);
    return 0;
}
