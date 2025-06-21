#include "debug.h"

#define READ_BYTE(chk, off)     ((chk)->codes[(off)++])
#define CHECK(chk, off, cnt)    ((chk)->count >= (off) + (cnt))
#define FMT_PREFIX              "[%04ld] <line:%02ld> %-10s"

#define op_return(chk, off)     op_func1("OP_RETURN", chk, off)
#define op_neg(chk, off)        op_func1("OP_NEG", chk, off)
#define op_add(chk, off)        op_func1("OP_ADD", chk, off)
#define op_sub(chk, off)        op_func1("OP_SUB", chk, off)
#define op_mul(chk, off)        op_func1("OP_MUL", chk, off)
#define op_div(chk, off)        op_func1("OP_DIV", chk, off)
#define op_not(chk, off)        op_func1("OP_NOT", chk, off)
#define op_equal(chk, off)      op_func1("OP_EQUAL", chk, off)
#define op_greater(chk, off)    op_func1("OP_GREATER", chk, off)
#define op_less(chk, off)       op_func1("OP_LESS", chk, off)
#define op_true(chk, off)       op_func1("OP_TRUE", chk, off)
#define op_false(chk, off)      op_func1("OP_FALSE", chk, off)
#define op_nil(chk, off)        op_func1("OP_NIL", chk, off)

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE size_t op_func1(const char *name, chunk_t *chunk, size_t offset);
PRIVATE size_t op_load(chunk_t *chunk, size_t offset);

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

PRIVATE size_t op_func1(const char *name, chunk_t *chunk, size_t offset)
{
    printf(FMT_PREFIX"\n", offset-1, chunk->lines[offset-1], name);
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
    case OP_LOAD:    offset = op_load(chunk, offset);    break;
    case OP_RETURN:  offset = op_return(chunk, offset);  break;
    case OP_NEG:     offset = op_neg(chunk, offset);     break;
    case OP_ADD:     offset = op_add(chunk, offset);     break;
    case OP_SUB:     offset = op_sub(chunk, offset);     break;
    case OP_MUL:     offset = op_mul(chunk, offset);     break;
    case OP_DIV:     offset = op_div(chunk, offset);     break;
    case OP_NOT:     offset = op_not(chunk, offset);     break;
    case OP_EQUAL:   offset = op_equal(chunk, offset);   break;
    case OP_GREATER: offset = op_greater(chunk, offset); break;
    case OP_LESS:    offset = op_less(chunk, offset);    break;
    case OP_TRUE:    offset = op_true(chunk, offset);    break;
    case OP_FALSE:   offset = op_false(chunk, offset);   break;
    case OP_NIL:     offset = op_nil(chunk, offset);     break;
    default:         unreachable("unknown opcode");
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

#undef op_return
#undef op_neg
#undef op_add
#undef op_sub
#undef op_mul
#undef op_div
#undef op_not
#undef op_equal
#undef op_greater
#undef op_less
#undef op_true
#undef op_false
#undef op_nil

