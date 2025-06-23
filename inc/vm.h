#ifndef VELO_VM_H
#define VELO_VM_H

#include "common.h"
#include "chunk.h"
#include "value.h"

#define STACK_SIZE 256

typedef struct {
    chunk_t chunk;
    uint8_t *pc;
    value_t ss[STACK_SIZE]; 
    value_t *sp;
    object_t *objects;
} vm_t;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} status_t;

PUBLIC void init_vm(vm_t *vm);
PUBLIC void free_vm(vm_t *vm);
PUBLIC status_t interpret(vm_t *vm, const char *source);

#endif // VELO_VM_H
