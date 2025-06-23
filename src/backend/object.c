#include <assert.h>
#include <string.h>

#include "object.h"

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE object_t *alloc_object(vm_t *vm, objtype_t type);
PRIVATE string_t *alloc_string(vm_t *vm, char *chars, size_t len);

/* ====================================================== *
 *             private function implementation            *
 * ====================================================== */

PRIVATE string_t *alloc_string(vm_t *vm, char *chars, size_t len)
{
    string_t *string = (string_t *) alloc_object(vm, OBJ_STRING);
    string->chars = chars;
    string->len = len;
    return string;
}

PRIVATE object_t *alloc_object(vm_t *vm, objtype_t type)
{
    size_t size;

    switch (type) {
    case OBJ_STRING: size = sizeof(string_t); break;
    default: unreachable("unknown type");
    }

    object_t *obj = malloc(size);
    assert(obj != NULL);
    obj->type = type;
    obj->next = vm->objects;
    vm->objects = obj;

    return obj;
}

/* ====================================================== *
 *             public function implementation             *
 * ====================================================== */

PUBLIC string_t *copy_string(vm_t *vm, const char *chars, size_t len)
{
    char *heap_chars = malloc(len + 1);
    assert(heap_chars != NULL);
    memcpy(heap_chars, chars, len);
    heap_chars[len] = '\0';
    return alloc_string(vm, heap_chars, len);
}

PUBLIC void print_object(value_t value)
{
    switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
        printf("%s", UNPACK_CSTRING(value));
        break;
    default: unreachable("unknown type");
    }
}

PUBLIC string_t *take_string(vm_t *vm, char *chars, size_t len)
{
    return alloc_string(vm, chars, len);
}

PUBLIC void free_object(object_t *obj)
{
    switch (obj->type) {
    case OBJ_STRING:
        string_t *string = (string_t *) obj;
        free(string->chars);
        free(obj);
        break;
    default: unreachable("unknown type");
    }
}
