#ifndef VELO_VALUE_H
#define VELO_VALUE_H

#include "common.h"

typedef struct object object_t;
typedef struct string string_t;

typedef enum {
    VT_BOOLEAN,
    VT_NIL,
    VT_NUMBER,
    VT_OBJECT,
} valtype_t;

typedef struct {
    valtype_t type;
    union {
        bool boolean;
        double number;
        object_t *obj;
    } as;
} value_t;

typedef struct {
    size_t count;
    size_t capacity;
    value_t *values;
} valpool_t;

#define IS_BOOLEAN(v) ((v).type == VT_BOOLEAN)
#define IS_NIL(v)     ((v).type == VT_NIL)
#define IS_NUMBER(v)  ((v).type == VT_NUMBER)
#define IS_OBJECT(v)  ((v).type == VT_OBJECT)

/* Pack */
#define PACK_BOOLEAN(v) ((value_t) {VT_BOOLEAN, .as.boolean = (v)})
#define PACK_NIL(v)     ((value_t) {VT_NIL, .as.number = 0})
#define PACK_NUMBER(v)  ((value_t) {VT_NUMBER, .as.number = (v)})
#define PACK_OBJECT(o)  ((value_t) {VT_OBJECT, .as.obj = (object_t*)(o)})

/* Unpack */
#define UNPACK_BOOLEAN(v) ((v).as.boolean)
#define UNPACK_NUMBER(v)  ((v).as.number)
#define UNPACK_OBJECT(v)  ((v).as.obj)

PUBLIC void init_value_pool(valpool_t *pool);
PUBLIC void free_value_pool(valpool_t *pool);
PUBLIC size_t add_value_to_pool(valpool_t *pool, value_t value);
PUBLIC void print_value(value_t value);
PUBLIC bool values_equal(value_t a, value_t b);

#endif // VELO_VALUE_H
