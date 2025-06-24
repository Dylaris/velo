#include <assert.h>
#include <string.h>

#include "object.h"
#include "table.h"

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE entry_t *find_entry(entry_t *entries, size_t capacity, string_t *key);
PRIVATE void adjust_capacity(table_t *table, size_t capacity);

/* ====================================================== *
 *             private function implementation            *
 * ====================================================== */

PRIVATE void adjust_capacity(table_t *table, size_t capacity)
{
    entry_t *entries = malloc(sizeof(entry_t)*capacity);
    assert(entries != NULL);
    for (size_t i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = PACK_NIL(0);
    }

    for (size_t i = 0; i < table->capacity; i++) {
        entry_t *entry = &table->entries[i];
        if (!entry->key) continue;

        entry_t *dest = find_entry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
    }

    if (table->entries) free(table->entries);
    table->entries = entries;
    table->capacity = capacity;
}

PRIVATE entry_t *find_entry(entry_t *entries, size_t capacity, string_t *key)
{
    size_t index = (size_t) key->hash % capacity;
    for (;;) {
        entry_t *entry = &entries[index];
        if (entry->key == key || entry->key == NULL) return entry;
        index = (index + 1) % capacity;
    }
}

/* ====================================================== *
 *             public function implementation             *
 * ====================================================== */

PUBLIC void init_table(table_t *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

PUBLIC void free_table(table_t *table)
{
    if (table->entries) free(table->entries);
    init_table(table);
}

PUBLIC bool table_set(table_t *table, string_t *key, value_t value)
{
    if (table->count+1 > table->capacity*TABLE_MAX_LOAD) {
        size_t capacity = table->capacity<8 ? 8 : 2*table->capacity;
        adjust_capacity(table, capacity);
    }

    entry_t *entry = find_entry(table->entries, table->capacity, key);
    bool is_new_key = entry->key == NULL;
    if (is_new_key) table->count++;

    entry->key = key;
    entry->value = value;
    return is_new_key;
}

PUBLIC void table_add_all(table_t *to, table_t *from)
{
    for (size_t i = 0; i < from->capacity; i++) {
        entry_t *entry = &from->entries[i];
        if (entry->key) table_set(to, entry->key, entry->value);
    }
}

PUBLIC bool table_get(table_t *table, string_t *key, value_t *value)
{
    if (table->count == 0) return false;

    entry_t *entry = find_entry(table->entries, table->capacity, key); 
    if (!entry->key) return false;

    *value = entry->value;
    return true;
}

PUBLIC bool table_delete(table_t *table, string_t *key)
{
    (void) table;
    (void) key;
    return true;
}

PUBLIC string_t *table_find_string(table_t *table, const char *chars,  
                                   size_t len, uint32_t hash)
{
    if (table->count == 0) return NULL;

    size_t index = (size_t) hash % table->capacity;
    for (;;) {
        entry_t *entry = &table->entries[index];
        if (!entry->key) {
            return NULL;
        } else if (entry->key->len == len && 
                   entry->key->hash == hash &&
                   memcmp(entry->key->chars, chars, len) == 0) {
            return entry->key;
        }
        index = (index + 1) % table->capacity;
    }
}
