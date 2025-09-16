#ifndef TETHER_ERROR_H
#define TETHER_ERROR_H

#include <stdio.h>

typedef struct {
    char* message; // heap-owned
    int line;      // 1-based
    int column;    // 1-based
} TetherError;

TetherError* tether_error_create(const char* message, int line, int column);
void tether_error_free(TetherError* err);
void tether_error_print(const TetherError* err, FILE* out);

#endif // TETHER_ERROR_H
