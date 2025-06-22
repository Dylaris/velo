#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "vm.h"
#include "debug.h"
#include "chunk.h"

#define DISASM

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

static bool run_script(const char *filename)
{
    vm_t vm;
    init_vm(&vm);

    char *source = read_file(filename);
    status_t ret = interpret(&vm, source);

#ifdef DISASM
    disasm_vm(&vm, "RUN SCRIPT");
#endif

    free_vm(&vm);

    return ret == INTERPRET_OK;
}

static bool repl(void)
{
    vm_t vm;
    init_vm(&vm);

    while (1) {
        printf("velo> ");
        fflush(stdout);

        char buf[1024] = {0};
        if (!fgets(buf, sizeof(buf), stdin)) goto err;
        if (strcmp(buf, "exit\n") == 0) goto ok;

        interpret(&vm, buf);

        free_vm(&vm);
    }

ok:
    free_vm(&vm);
    return true;

err:
    free_vm(&vm);
    return false;
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        return repl() ? 0 : 1;
    } else {
        return run_script(argv[1]) ? 0 : 1;
    }
}
