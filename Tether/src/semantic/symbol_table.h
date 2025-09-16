#ifndef TETHER_SYMBOL_TABLE_H
#define TETHER_SYMBOL_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_TYPE
} SymbolKind;

typedef struct Symbol {
    char* name; // heap-owned
    SymbolKind kind;
    void* info; // opaque pointer for type/signature/metadata
    struct Symbol* next; // for chaining in buckets
} Symbol;

typedef struct SymbolTable {
    size_t bucket_count;
    Symbol** buckets; // array of bucket_count pointers
    struct SymbolTable* parent; // enclosing scope
} SymbolTable;

SymbolTable* symbol_table_create(size_t bucket_count, SymbolTable* parent);
void symbol_table_free(SymbolTable* table);
int symbol_table_insert(SymbolTable* table, const char* name, SymbolKind kind, void* info);
Symbol* symbol_table_lookup(SymbolTable* table, const char* name);

#endif // TETHER_SYMBOL_TABLE_H
