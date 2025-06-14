#include "compiler.h"
#include "lexer.h"

/* ====================================================== *
 *              public function implementation            *
 * ====================================================== */

PUBLIC int compile(vm_t *vm, const char *source)
{
    (void) vm;

    lexer_t lexer = {0};
    init_lexer(&lexer, source);

    int status = 0;
    while (1) {
        token_t token = scan_token(&lexer);
        printf("<line:%04ld> %-30s %.*s\n", token.line,
                token_to_string(token), (int) token.length, token.start);
        if (token.type == TOKEN_ERROR) status = 1;
        if (token.type == TOKEN_EOF) break;
    }

    return status;
}
