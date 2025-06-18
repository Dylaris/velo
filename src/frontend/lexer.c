#include <string.h>

#include "lexer.h"

#define IS_WHITESPACE(ch)           (ch == '\n' || ch == '\t' || ch == '\r' || ch == ' ')
#define IS_COMMENT(lexer)           (peek(lexer) == '/' && peek_next(lexer) == '/')
#define IS_DIGIT(ch)                (ch >= '0' && ch <= '9')
#define IS_ALPHA(ch)                ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
#define IS_ALNUM(ch)                (IS_DIGIT(ch) || IS_ALPHA(ch))
#define IS_STRING_PREFIX(ch)        (ch == '"')
#define IS_IDENTIFIER_PREFIX(ch)    (ch == '_' || IS_ALPHA(ch))

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE token_t make_token(lexer_t *lexer, toktype_t type);
PRIVATE token_t error_token(lexer_t *lexer, const char *msg);
PRIVATE void skip_whitespace(lexer_t *lexer);
PRIVATE void skip_comment(lexer_t *lexer);
PRIVATE token_t scan_number(lexer_t *lexer);
PRIVATE token_t scan_string(lexer_t *lexer);
PRIVATE token_t scan_identifier(lexer_t *lexer);
PRIVATE bool is_at_end(lexer_t *lexer);
PRIVATE char advance(lexer_t *lexer);
PRIVATE char peek(lexer_t *lexer);
PRIVATE char peek_next(lexer_t *lexer);
PRIVATE bool match(lexer_t *lexer, char expected);
PRIVATE toktype_t get_identifier_type(lexer_t *lexer);

/* ====================================================== *
 *             private function implementation            *
 * ====================================================== */

PRIVATE toktype_t get_identifier_type(lexer_t *lexer)
{
    switch (lexer->start[0]) {
    case 'v': return TOKEN_VAR;
    case 'r': return TOKEN_RETURN;
    case 'p': return TOKEN_PRINT;
    default:  return TOKEN_IDENTIFIER;
    }
}

PRIVATE token_t scan_number(lexer_t *lexer)
{
    while (IS_DIGIT(peek(lexer))) advance(lexer);

    /* Look for a fractional part */
    if (peek(lexer) == '.' && IS_DIGIT(peek_next(lexer))) {
        advance(lexer); // consume the '.'
        while (IS_DIGIT(peek(lexer))) advance(lexer);
    }

    return make_token(lexer, TOKEN_NUMBER);
}

PRIVATE token_t scan_string(lexer_t *lexer)
{
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') lexer->line++;
        advance(lexer);
    }
    
    if (is_at_end(lexer)) return error_token(lexer, "unterminated string");
    
    advance(lexer); // consume the closed quote
    return make_token(lexer, TOKEN_STRING);
}

PRIVATE token_t scan_identifier(lexer_t *lexer)
{
    while (peek(lexer) == '_' || IS_ALNUM(peek(lexer))) advance(lexer);
    return make_token(lexer, get_identifier_type(lexer));
}

PRIVATE bool match(lexer_t *lexer, char expected)
{
    if (is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    lexer->current++;
    return true;
}

PRIVATE void skip_whitespace(lexer_t *lexer)
{
    while (1) {
        char c = peek(lexer);
        switch (c) {
        case '\n':
        case '\t':
        case '\r':
        case ' ':
            if (c == '\n') lexer->line++;
            advance(lexer);
            break;
        default: 
            lexer->start = lexer->current;
            return;
        }
    }
}

PRIVATE void skip_comment(lexer_t *lexer)
{
    if (IS_COMMENT(lexer)) {
        advance(lexer);
        advance(lexer);
        while (1) {
            char c = advance(lexer);
            if (c == '\n') {
                lexer->line++;
                if (IS_COMMENT(lexer)) skip_comment(lexer);
                lexer->start = lexer->current;
                return;
            }
        }
    }
}

PRIVATE char advance(lexer_t *lexer)
{
    return *lexer->current++;
}

PRIVATE char peek(lexer_t *lexer)
{
    return *lexer->current;
}

PRIVATE char peek_next(lexer_t *lexer)
{
    if (is_at_end(lexer)) return '\0';
    return *(lexer->current + 1);
}

PRIVATE bool is_at_end(lexer_t *lexer)
{
    return *lexer->current == '\0';
}

PRIVATE token_t error_token(lexer_t *lexer, const char *msg)
{
    return (token_t) {
        .type   = TOKEN_ERROR,
        .start  = msg,
        .length = strlen(msg),
        .line   = lexer->line
    };
}

PRIVATE token_t make_token(lexer_t *lexer, toktype_t type)
{
    return (token_t) {
        .type   = type,
        .start  = lexer->start,
        .length = lexer->current - lexer->start,
        .line   = lexer->line
    };
}

/* ====================================================== *
 *             public function implementation             *
 * ====================================================== */

PUBLIC void init_lexer(lexer_t *lexer, const char *source)
{
    lexer->start = source;
    lexer->current = source; 
    lexer->line = 1;
}

PUBLIC token_t scan_token(lexer_t *lexer)
{
    while (1) {
        lexer->start = lexer->current;

        while (IS_WHITESPACE(peek(lexer)) || IS_COMMENT(lexer)) {
            skip_whitespace(lexer);
            skip_comment(lexer);
            if (is_at_end(lexer)) return make_token(lexer, TOKEN_EOF);
        }

        if (is_at_end(lexer)) return make_token(lexer, TOKEN_EOF);
        char c = advance(lexer);

        if (IS_DIGIT(c)) return scan_number(lexer);
        if (IS_STRING_PREFIX(c)) return scan_string(lexer);
        if (IS_IDENTIFIER_PREFIX(c)) return scan_identifier(lexer);

        switch (c) {
        case '+': return make_token(lexer, TOKEN_PLUS);
        case '-': return make_token(lexer, TOKEN_MINUS);
        case '*': return make_token(lexer, TOKEN_STAR);
        case '/': return make_token(lexer, TOKEN_SLASH);
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case '{': return make_token(lexer, TOKEN_LBRACE);
        case '}': return make_token(lexer, TOKEN_RBRACE);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case '!': return make_token(lexer,
                          match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return make_token(lexer,
                          match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '>': return make_token(lexer,
                          match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '<': return make_token(lexer,
                          match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        default:  printf("%d\n", c); return error_token(lexer, "unexpected character");
        }
    }
}

PUBLIC char *token_to_string(token_t token)
{
    switch (token.type) {
    case TOKEN_PLUS:            return "TOKEN_PLUS";
    case TOKEN_MINUS:           return "TOKEN_MINUS";
    case TOKEN_STAR:            return "TOKEN_STAR";
    case TOKEN_SLASH:           return "TOKEN_SLASH";
    case TOKEN_BANG:            return "TOKEN_BANG";
    case TOKEN_BANG_EQUAL:      return "TOKEN_BANG_EQUAL";
    case TOKEN_EQUAL:           return "TOKEN_EQUAL";
    case TOKEN_EQUAL_EQUAL:     return "TOKEN_EQUAL_EQUA";
    case TOKEN_GREATER:         return "TOKEN_GREATER";
    case TOKEN_GREATER_EQUAL:   return "TOKEN_GREATER_EQUAL";
    case TOKEN_LESS:            return "TOKEN_LESS";
    case TOKEN_LESS_EQUAL:      return "TOKEN_LESS_EQUA";
    case TOKEN_ERROR:           return "TOKEN_ERROR";
    case TOKEN_EOF:             return "TOKEN_EOF";
    case TOKEN_LPAREN:          return "TOKEN_LPAREN";
    case TOKEN_RPAREN:          return "TOKEN_RPAREN";
    case TOKEN_LBRACE:          return "TOKEN_LBRACE";
    case TOKEN_RBRACE:          return "TOKEN_RBRACE";
    case TOKEN_SEMICOLON:       return "TOKEN_SEMICOLON";
    case TOKEN_COMMA:           return "TOKEN_COMMA";
    case TOKEN_DOT:             return "TOKEN_DOT";
    case TOKEN_NUMBER:          return "TOKEN_NUMBER";
    case TOKEN_STRING:          return "TOKEN_STRING";
    case TOKEN_IDENTIFIER:      return "TOKEN_IDENTIFIER";
    case TOKEN_VAR:             return "TOKEN_VAR";
    case TOKEN_RETURN:          return "TOKEN_RETURN";
    case TOKEN_PRINT:           return "TOKEN_PRINT";
    default:                    return "unknown token";
    }
}

#undef IS_COMMENT
#undef IS_WHITESPACE
#undef IS_ALPHA
#undef IS_DIGIT
#undef IS_STRING_PREFIX
#undef IS_IDENTIFIER_PREFIX

