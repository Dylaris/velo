#include <assert.h>
#include <string.h>

#include "object.h"
#include "table.h"

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE object_t *alloc_object(vm_t *vm, objtype_t type);
PRIVATE string_t *alloc_string(vm_t *vm, char *chars,
                               size_t len, uint32_t hash);
PRIVATE uint32_t hash_string(const char* key, size_t len);

/* ====================================================== *
 *             private function implementation            *
 * ====================================================== */

PRIVATE uint32_t hash_string(const char* key, size_t len)
{
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < len; i++) {
        hash ^= (uint8_t) key[i];
        hash *= 16777619;
    }
    return hash;
}

PRIVATE string_t *alloc_string(vm_t *vm, char *chars,
                               size_t len, uint32_t hash)
{
    string_t *string = (string_t *) alloc_object(vm, OBJ_STRING);
    string->chars = chars;
    string->len = len;
    string->hash = hash;
    table_set(&vm->strings, string, PACK_NIL(0));
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
    uint32_t hash = hash_string(chars, len);
    string_t *interned = table_find_string(&vm->strings, chars, len, hash);
    if (interned) return interned;

    char *heap_chars = malloc(len + 1);
    assert(heap_chars != NULL);
    memcpy(heap_chars, chars, len);
    heap_chars[len] = '\0';
    return alloc_string(vm, heap_chars, len, hash);
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
    uint32_t hash = hash_string(chars, len);
    string_t *interned = table_find_string(&vm->strings, chars, len, hash);
    if (interned) {
        if (chars) free(chars);
        return interned;
    }

    return alloc_string(vm, chars, len, hash);
}

PUBLIC void free_object(object_t *obj)
{
    switch (obj->type) {
    case OBJ_STRING: {
        string_t *pstr = (string_t *) obj;
        free(pstr->chars);
        free(obj);
        break;
    }
    default: unreachable("unknown type");
    }
}
