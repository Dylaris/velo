#include "debug.h"
#include "vm.h"

#define READ_BYTE(chk, off)     ((chk)->codes[(off)++])
#define CHECK(chk, off, cnt)    ((chk)->count >= (off) + (cnt))

PRIVATE size_t op_load(chunk_t *chunk, size_t offset);
PRIVATE size_t op_return(chunk_t *chunk, size_t offset);

PRIVATE size_t op_load(chunk_t *chunk, size_t offset)
{
    if (!CHECK(chunk, offset, 1)) fatal("OP_LOAD without constant index");
    uint8_t index = READ_BYTE(chunk, offset); 
    printf("<line:%02zu> %-10s %02X '", chunk->lines[offset-1], "OP_LOAD", index);
    if (index >= chunk->constants.count) fatal("OP_LOAD index overflow");
    print_value(chunk->constants.values[index]);
    printf("'\n");

    return offset;
}

PRIVATE size_t op_return(chunk_t *chunk, size_t offset)
{
    if (!CHECK(chunk, offset, 1)) fatal("OP_RETURN without exit code");
    uint8_t exit_code = READ_BYTE(chunk, offset); 
    printf("<line:%02zu> %-10s %02x\n", chunk->lines[offset-1], "OP_RETURN", exit_code);

    return offset;
}

PUBLIC void disasm_chunk(chunk_t *chunk, const char *name)
{
    printf("=== %s ===\n", name ? name : "(null)");

    size_t offset = 0;
    while (offset < chunk->count) {
        offset = disasm_instruction(chunk, offset);
    }
}

PUBLIC size_t disasm_instruction(chunk_t *chunk, size_t offset)
{
    opcode_t opcode = READ_BYTE(chunk, offset);
    switch (opcode) {
    case OP_LOAD:   offset = op_load(chunk, offset); break;
    case OP_RETURN: offset = op_return(chunk, offset); break;
    default:        unreachable("unknown opcode");
    }

    return offset;
}

#undef READ_BYTE
#undef CHECK
