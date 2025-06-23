#ifndef VELO_OBJECT_H 
#define VELO_OBJECT_H 

#include "common.h"
#include "value.h"
#include "vm.h"

typedef enum {
    OBJ_STRING,
} objtype_t;

struct object {
    objtype_t type;
    struct object *next;
};

struct string {
    struct object obj;
    size_t len;
    char *chars;
};

#define OBJ_TYPE(v)         (UNPACK_OBJECT(v)->type)
#define IS_STRING(v)        check_objtype(v, OBJ_STRING)
#define UNPACK_STRING(v)    ((string_t*)UNPACK_OBJECT(v))
#define UNPACK_CSTRING(v)   (((string_t*)UNPACK_OBJECT(v))->chars)

/* The difference between copy_string and take_string is the 
   ownership of 'chars'. In copy_string, we assume 'chars' shouldn't
   be changed by caller. And in take_string, we assume 'chars' belong
   to the caller. */
PUBLIC string_t *copy_string(vm_t *vm, const char *chars, size_t len);
PUBLIC string_t *take_string(vm_t *vm, char *chars, size_t len);
PUBLIC void print_object(value_t value);
PUBLIC void free_object(object_t *obj);

static inline bool check_objtype(value_t value, objtype_t type)
{
    return IS_OBJECT(value) && OBJ_TYPE(value) == type;
}

#endif // VELO_OBJECT_H 
