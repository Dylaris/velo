#ifndef VELO_VALUE_H
#define VELO_VALUE_H

#include "common.h"

typedef double value_t;

typedef struct {
    size_t count;
    size_t capacity;
    value_t *values;
} value_pool_t;

PUBLIC void init_value_pool(value_pool_t *pool);
PUBLIC void free_value_pool(value_pool_t *pool);
PUBLIC size_t add_value_to_pool(value_pool_t *pool, value_t value);
PUBLIC void print_value(value_t value);

#endif // VELO_VALUE_H
