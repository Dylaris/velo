#ifndef VELO_VM_H
#define VELO_VM_H

#include "common.h"
#include "chunk.h"

typedef struct {
    chunk_t chunk;
    size_t pc;
} vm_t;

PUBLIC void init_vm(vm_t *vm);
PUBLIC void free_vm(vm_t *vm);

#endif // VELO_VM_H
