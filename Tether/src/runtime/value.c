#include "value.h"
#include <string.h>
#include <stdlib.h>

Value value_null() {
    Value v; v.type = VAL_NULL; return v;
}

Value value_bool(int b) {
    Value v; v.type = VAL_BOOL; v.as.i64 = b ? 1 : 0; return v;
}

Value value_i64(int64_t x) {
    Value v; v.type = VAL_INT64; v.as.i64 = x; return v;
}

Value value_f64(double x) {
    Value v; v.type = VAL_FLOAT64; v.as.f64 = x; return v;
}

Value value_string(const char* s) {
    Value v; v.type = VAL_STRING; if (!s) { v.as.string = NULL; return v; }
    v.as.string = strdup(s);
    return v;
}

void value_free(Value* v) {
    if (!v) return;
    switch (v->type) {
        case VAL_STRING:
            if (v->as.string) free(v->as.string);
            break;
        case VAL_ARRAY:
            if (v->as.array.items) free(v->as.array.items);
            break;
        default:
            break;
    }
    v->type = VAL_NULL;
}
