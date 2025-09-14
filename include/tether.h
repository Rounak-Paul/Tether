// Minimal public header for Tether runtime
#pragma once

#include <stdint.h>

typedef uint64_t usize;

typedef enum {
    T_I32,
    T_F32,
    T_BOOL,
    T_ARRAY,
    T_UNKNOWN,
} TypeTag;

// Node capabilities struct (JSON in practice)
typedef struct {
    int cpu_cores;
    int has_gpu;
} NodeCapabilities;
