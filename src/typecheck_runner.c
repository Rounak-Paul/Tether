#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "runtime_api.h"

extern int typecheck_file(const char *path);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: typecheck_runner <file1.t> ...\n");
        return 2;
    }
    int failures = 0;
    for (int i = 1; i < argc; ++i) {
        int r = typecheck_file(argv[i]);
        if (r != 0) failures++;
    }
    if (failures) {
        printf("TYPECHECK: %d failed\n", failures);
        return 1;
    }
    printf("TYPECHECK: all passed\n");
    return 0;
}
