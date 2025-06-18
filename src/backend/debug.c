#include "debug.h"

#define READ_BYTE(chk, off)     ((chk)->codes[(off)++])
#define CHECK(chk, off, cnt)    ((chk)->count >= (off) + (cnt))
#define FMT_PREFIX              "[%04ld] <line:%02ld> %-10s"

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE size_t op_load(chunk_t *chunk, size_t offset);
PRIVATE size_t op_return(chunk_t *chunk, size_t offset);
PRIVATE size_t op_neg(chunk_t *chunk, size_t offset);
PRIVATE size_t op_add(chunk_t *chunk, size_t offset);
PRIVATE size_t op_sub(chunk_t *chunk, size_t offset);
PRIVATE size_t op_mul(chunk_t *chunk, size_t offset);
PRIVATE size_t op_div(chunk_t *chunk, size_t offset);


/* ====================================================== *
 *           private function implementation              *
 * ====================================================== */

PRIVATE size_t op_load(chunk_t *chunk, size_t offset)
{
    if (!CHECK(chunk, offset, 1)) fatal("OP_LOAD without constant index");
    uint8_t index = READ_BYTE(chunk, offset); 
    printf(FMT_PREFIX" %02X '", offset-2,
            chunk->lines[offset-1], "OP_LOAD", index);
    if (index >= chunk->constants.count) fatal("OP_LOAD index overflow");
    print_value(chunk->constants.values[index]);
    printf("'\n");

    return offset;
}

PRIVATE size_t op_return(chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1,
            chunk->lines[offset-1], "OP_RETURN");
    return offset;
}

PRIVATE size_t op_neg(chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1,
            chunk->lines[offset-1], "OP_NEG");
    return offset;
}

PRIVATE size_t op_add(chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1,
            chunk->lines[offset-1], "OP_ADD");
    return offset;
}

PRIVATE size_t op_sub(chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1,
            chunk->lines[offset-1], "OP_SUB");
    return offset;
}

PRIVATE size_t op_mul(chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1,
            chunk->lines[offset-1], "OP_MUL");
    return offset;
}

PRIVATE size_t op_div(chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1,
            chunk->lines[offset-1], "OP_DIV");
    return offset;
}

/* ====================================================== *
 *           public function implementation               *
 * ====================================================== */

PUBLIC void disasm_vm(vm_t *vm, const char *name)
{
    printf("=== %s ===\n", name ? name : "(null)");

    chunk_t *chunk = &vm->chunk;
    dump_chunk(chunk);
    printf("\n");

    size_t offset = 0;
    while (offset < chunk->count) {
        offset = disasm_instruction(chunk, offset);
    }
}

PUBLIC size_t disasm_instruction(chunk_t *chunk, size_t offset)
{
    opcode_t opcode = READ_BYTE(chunk, offset);
    switch (opcode) {
    case OP_LOAD:   offset = op_load(chunk, offset);   break;
    case OP_RETURN: offset = op_return(chunk, offset); break;
    case OP_NEG:    offset = op_neg(chunk, offset);    break;
    case OP_ADD:    offset = op_add(chunk, offset);    break;
    case OP_SUB:    offset = op_sub(chunk, offset);    break;
    case OP_MUL:    offset = op_mul(chunk, offset);    break;
    case OP_DIV:    offset = op_div(chunk, offset);    break;
    default:        unreachable("unknown opcode");
    }

    return offset;
}

PUBLIC void dump_stack(value_t *ss, size_t size)
{
    printf("\t");
    for (size_t i = 0; i < size && i < STACK_SIZE; i++) {
        printf("[ ");
        print_value(ss[i]);
        printf(" ]  ");
    }
    printf("\n");
}

PUBLIC void dump_chunk(chunk_t *chunk)
{
    for (size_t i = 0; i < chunk->count; i += 8) {
        printf("%04ld >", i);
        for (size_t j = i; j < i+8 && j < chunk->count; j++) {
            printf(" %02X", chunk->codes[j]);
        }
        printf("\n");
    }
}

#undef READ_BYTE
#undef CHECK
#undef FMT_PREFIX
