#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "vm.h"
#include "debug.h"
#include "chunk.h"

static char *read_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: can't open the file %s\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *source = NULL;
    if (size > 0) {
        source = malloc(size + 1);
        assert(source != NULL);
        assert(fread(source, 1, size, fp) == (size_t) size);
        source[size] = '\0';
    }

    fclose(fp);
    return source;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <FILE>\n", argv[0]);
        return 1;
    }

    vm_t vm;
    init_vm(&vm);

    char *source = read_file(argv[1]);
    interpret(&vm, source);

    disasm_vm(&vm, "test");

    free_vm(&vm);
    return 0;
}
