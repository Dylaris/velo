#include <stdio.h>

#include "vm.h"
#include "debug.h"
#include "chunk.h"

int main(void)
{
    vm_t vm;
    init_vm(&vm);

    uint8_t index;

    write_code_to_chunk(&vm.chunk, OP_LOAD, 123);
    index = add_constant_to_chunk(&vm.chunk, 12.3);
    write_code_to_chunk(&vm.chunk, index, 123);

    write_code_to_chunk(&vm.chunk, OP_LOAD, 123);
    index = add_constant_to_chunk(&vm.chunk, 3);
    write_code_to_chunk(&vm.chunk, index, 123);
    
    write_code_to_chunk(&vm.chunk, OP_LOAD, 123);
    index = add_constant_to_chunk(&vm.chunk, -1.1);
    write_code_to_chunk(&vm.chunk, index, 123);

    write_code_to_chunk(&vm.chunk, OP_RETURN, 123);
    write_code_to_chunk(&vm.chunk, 0, 123);

    disasm_chunk(&vm.chunk, "test");

    free_vm(&vm);
    return 0;
}
