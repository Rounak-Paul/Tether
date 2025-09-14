#include <stdio.h>
#include "tether.h"

int net_register(const char *controller_addr, NodeCapabilities *caps) {
    printf("net: pretend register to %s with cores=%d gpu=%d\n", controller_addr, caps->cpu_cores, caps->has_gpu);
    return 0;
}
