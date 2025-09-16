#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>

static size_t hash_str(const char* s) {
    // simple djb2
    unsigned long hash = 5381;
    int c;
    while ((c = *s++)) hash = ((hash << 5) + hash) + (unsigned char)c;
    return (size_t)hash;
}

SymbolTable* symbol_table_create(size_t bucket_count, SymbolTable* parent) {
    SymbolTable* t = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!t) return NULL;
    t->bucket_count = bucket_count;
    t->buckets = (Symbol**)calloc(bucket_count, sizeof(Symbol*));
    if (!t->buckets) { free(t); return NULL; }
    t->parent = parent;
    return t;
}

void symbol_table_free(SymbolTable* table) {
    if (!table) return;
    for (size_t i = 0; i < table->bucket_count; ++i) {
        Symbol* s = table->buckets[i];
        while (s) {
            Symbol* next = s->next;
            if (s->name) free(s->name);
            free(s);
            s = next;
        }
    }
    free(table->buckets);
    free(table);
}

int symbol_table_insert(SymbolTable* table, const char* name, SymbolKind kind, void* info) {
    if (!table || !name) return 0;
    size_t idx = hash_str(name) % table->bucket_count;
    Symbol* cur = table->buckets[idx];
    while (cur) {
        if (strcmp(cur->name, name) == 0) return 0; // already exists
        cur = cur->next;
    }
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    if (!s) return 0;
    s->name = strdup(name);
    s->kind = kind;
    s->info = info;
    s->next = table->buckets[idx];
    table->buckets[idx] = s;
    return 1;
}

Symbol* symbol_table_lookup(SymbolTable* table, const char* name) {
    SymbolTable* t = table;
    while (t) {
        size_t idx = hash_str(name) % t->bucket_count;
        Symbol* cur = t->buckets[idx];
        while (cur) {
            if (strcmp(cur->name, name) == 0) return cur;
            cur = cur->next;
        }
        t = t->parent;
    }
    return NULL;
}
