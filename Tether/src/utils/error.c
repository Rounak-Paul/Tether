#include "error.h"
#include <stdlib.h>
#include <string.h>

TetherError* tether_error_create(const char* message, int line, int column) {
    if (!message) return NULL;
    TetherError* e = (TetherError*)malloc(sizeof(TetherError));
    if (!e) return NULL;
    e->message = strdup(message);
    e->line = line;
    e->column = column;
    return e;
}

void tether_error_free(TetherError* err) {
    if (!err) return;
    if (err->message) free(err->message);
    free(err);
}

void tether_error_print(const TetherError* err, FILE* out) {
    if (!err || !out) return;
    fprintf(out, "Error: %s (line %d, col %d)\n", err->message ? err->message : "", err->line, err->column);
}
