#ifndef VELO_DEBUG_H
#define VELO_DEBUG_H

#include "common.h"
#include "vm.h"

PUBLIC void disasm_vm(vm_t *vm, const char *name);
PUBLIC size_t disasm_instruction(chunk_t *chunk, size_t offset);
PUBLIC void dump_stack(value_t *ss, size_t size);
PUBLIC void dump_chunk(chunk_t *chunk);

#endif // VELO_DEBUG_H
