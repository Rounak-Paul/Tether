#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tether.h"
#include "runtime_api.h"

int main(int argc, char **argv) {
    printf("tether-node starting...\n");

    // very small CLI handling
    if (argc > 1 && strcmp(argv[1], "--register") == 0) {
        const char *controller = NULL;
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--controller") == 0 && i+1 < argc) controller = argv[i+1];
        }
        if (!controller) controller = "localhost:9000";
        printf("Registering with controller %s (simulated)\n", controller);
        // heartbeat loop (stub)
        for (int i = 0; i < 5; ++i) {
            printf("heartbeat %d\n", i);
            sleep(1);
        }
        printf("registration demo complete\n");
        return 0;
    }

    printf("no command given; starting runtime demo\n");
    runtime_init();
    return 0;
}
