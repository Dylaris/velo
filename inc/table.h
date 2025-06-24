#ifndef VELO_TABLE_H
#define VELO_TABLE_H

#include "common.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

typedef struct {
    string_t *key;
    value_t value;
} entry_t;

typedef struct {
    size_t count;
    size_t capacity;
    entry_t *entries;
} table_t;

PUBLIC void init_table(table_t *table);
PUBLIC void free_table(table_t *table);
PUBLIC bool table_set(table_t *table, string_t *key, value_t value);
PUBLIC bool table_get(table_t *table, string_t *key, value_t *value);
PUBLIC void table_add_all(table_t *to, table_t *from);
PUBLIC bool table_delete(table_t *table, string_t *key);
PUBLIC string_t *table_find_string(table_t *table, const char *chars,  
                                   size_t len, uint32_t hash);

#endif // VELO_TABLE_H
