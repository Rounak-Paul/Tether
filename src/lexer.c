#include <stdio.h>
#include "tether.h"

// Very small lexer stub for Phase 1
int lexer_init(const char *path) {
    printf("lexer: init %s\n", path);
    return 0;
}
