#ifndef TETHER_VALUE_H
#define TETHER_VALUE_H

#include <stdint.h>
#include <stdlib.h>

typedef enum {
    VAL_NULL,
    VAL_BOOL,
    VAL_INT64,
    VAL_UINT64,
    VAL_FLOAT64,
    VAL_STRING,
    VAL_ARRAY,
    VAL_OBJECT,
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        int64_t i64;
        uint64_t u64;
        double f64;
        char* string; // heap-owned
        struct {
            struct Value* items;
            size_t length;
        } array;
        void* object; // runtime-specific representation
    } as;
} Value;

Value value_null();
Value value_bool(int b);
Value value_i64(int64_t v);
Value value_f64(double v);
Value value_string(const char* s);
void value_free(Value* v);

#endif // TETHER_VALUE_H
