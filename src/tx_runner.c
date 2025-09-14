#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ast.h"

extern AST *parser_parse_ast(const char *src);
extern int interp_execute(AST *ast);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: tx-runner <file.tx>\n");
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
    if (!ast) { fprintf(stderr, "parse error\n"); free(buf); return 1; }

    int rc = interp_execute(ast);
    ast_free(ast);
    free(buf);
    return rc;
}
