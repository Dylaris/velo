#include "value.h"

PUBLIC void init_value_pool(value_pool_t *pool)
{
    pool->count    = 0;
    pool->capacity = 0;
    pool->values   = NULL;
}

PUBLIC void free_value_pool(value_pool_t *pool)
{
    if (pool->values) free(pool->values);
    init_value_pool(pool);
}

PUBLIC size_t add_value_to_pool(value_pool_t *pool, value_t value)
{
    if (pool->capacity <= pool->count) {
        pool->capacity = (pool->capacity==0) ? 10 : 2*pool->capacity;
        pool->values = realloc(pool->values, pool->capacity*sizeof(value_t));
        if (!pool->values) fatal("out of memory");
    }

    pool->values[pool->count] = value;
    return pool->count++;
}

PUBLIC void print_value(value_t value)
{
    printf("%g", value);
}
