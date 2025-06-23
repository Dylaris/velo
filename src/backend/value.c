#include <string.h>

#include "value.h"
#include "object.h"

/* ====================================================== *
 *          public function implementation                *
 * ====================================================== */

PUBLIC void init_value_pool(valpool_t *pool)
{
    pool->count    = 0;
    pool->capacity = 0;
    pool->values   = NULL;
}

PUBLIC void free_value_pool(valpool_t *pool)
{
    if (pool->values) free(pool->values);
    init_value_pool(pool);
}

PUBLIC size_t add_value_to_pool(valpool_t *pool, value_t value)
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
    switch (value.type) {
    case VT_BOOLEAN:
        printf(UNPACK_BOOLEAN(value) ? "true" : "false");
        break;
    case VT_NIL:
        printf("nil");
        break;
    case VT_NUMBER:
        printf("%g", UNPACK_NUMBER(value));
        break;
    case VT_OBJECT:
        print_object(value);
        break;
    default:
        unreachable("unknown value type");
    }
}

PUBLIC bool values_equal(value_t a, value_t b)
{
    if (a.type != b.type) return false;
    switch (a.type) {
    case VT_BOOLEAN: return UNPACK_BOOLEAN(a) == UNPACK_BOOLEAN(b);
    case VT_NUMBER:  return UNPACK_NUMBER(a) == UNPACK_NUMBER(b);
    case VT_NIL:     return true;
    case VT_OBJECT: {
        string_t *sa = UNPACK_STRING(a);
        string_t *sb = UNPACK_STRING(b);
        return sa->len == sb->len &&
               memcmp(sa->chars, sb->chars, sa->len) == 0;
    }
    default: unreachable("unknown type");
    }
}
