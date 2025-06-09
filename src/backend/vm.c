#include "vm.h"

PRIVATE inst_t read_instruction(vm_t *vm);

PRIVATE inst_t read_instruction(vm_t *vm)
{
#define READ_BYTE() (vm->chunk.codes[vm->pc++])

    inst_t res = {0};
    res.opcode = READ_BYTE();

    switch (res.opcode) {
    case OP_LOAD:   res.operand.index = READ_BYTE(); break;
    case OP_RETURN: res.operand.status = READ_BYTE(); break;
    default:        unreachable("unknown opcode");
    }

    return res;

#undef READ_BYTE
}

PUBLIC void init_vm(vm_t *vm)
{
    init_chunk(&vm->chunk);
    vm->pc = 0;
}

PUBLIC void free_vm(vm_t *vm)
{
    free_chunk(&vm->chunk);
    init_vm(vm);
}


