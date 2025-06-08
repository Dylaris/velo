/*
 * Author: Dylaris
 * Copyright (c) 2025
 * License: MIT
 * Date: 2025-06-05
 *
 * All rights reserved
 */

/*
 * Usage: Define a few macros to enable the corresponding functions
 *
 *   ZD_STATIC            Restricts the function scope to static
 *   ZD_IMPLEMENTATION    Includes function implementation when defined, otherwise only the header is included
 *
 * '+' means you can use it in 'linux' and 'windows', otherwise only 'linux'
 *
 * + ZD_BUILD             Build the c project using only c 
 * + ZD_LOG               Simple logging for information
 * + ZD_FS                Some operations about file and directory
 * + ZD_WILDCARD          Wildcard ( '*', '?' )
 * + ZD_COMMAND_LINE      Some operations about command line (option, ...)
 * + ZD_DS_DYNAMIC_ARRAY  Dynamic array
 * + ZD_DS_STRING         String
 * + ZD_DS_STACK          Stack
 * + ZD_DS_QUEUE          Queue (based on linked-list)
 *
 * !!! NOTE !!!
 *
 * Since `zd.h` implements a generic data structure, the elements you push into it should be pointers. 
 * For example, if you want to push an integer, you should pass an (int *). 
 *
 * Because strings are not of fixed length, they cannot be directly used as elements of these generic data structures. 
 * Therefore, we use (char **) as the element type of the data structure, pointing to the string. 
 * Since what we actually want to push into the data structure is (char **), we need to pass (char ***).
 * If you find this cumbersome, you can simply use the implemented struct zd_string.
 */

#ifndef ZD_H
#define ZD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#ifndef ZD_DEF
  #ifdef ZD_STATIC
    #define ZD_DEF static
  #else
    #define ZD_DEF extern
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* MODULE: ZD_WILDCARD */

ZD_DEF bool zd_wildcard_match(const char *str, const char *pattern);
ZD_DEF void zd_wildcard_capture(const char *str, const char *pattern,
                                size_t *start, size_t *len);

#define wc_match    zd_wildcard_match
#define wc_capture  zd_wildcard_capture

/* MODULE: ZD_LOG */

#define LOG_INFO  1
#define LOG_ERROR 2
#define LOG_GOOD  3
#define LOG_TODO  4
#define LOG_FATAL 5

ZD_DEF void zd_log(int type, const char *fmt, ...);

#define log zd_log

/* MODULE: ZD_DS_DYNAMIC_ARRAY */

struct zd_dyna {
    void *base;
    size_t count;
    size_t capacity;
    void (*clear_item)(void *);
    size_t size;    /* size of each element */
    size_t pos;
};

ZD_DEF void zd_dyna_init(struct zd_dyna *da, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_dyna_append(struct zd_dyna *da, void *elem);
ZD_DEF void zd_dyna_insert(struct zd_dyna *da, size_t idx, void *elem);
ZD_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx);
ZD_DEF void zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem);
ZD_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx);
ZD_DEF void zd_dyna_free(struct zd_dyna *da);
ZD_DEF void *zd_dyna_next(struct zd_dyna *da);

typedef struct zd_dyna  dyna_t;

#define dyna_init       zd_dyna_init
#define dyna_append     zd_dyna_append
#define dyna_insert     zd_dyna_insert
#define dyna_remove     zd_dyna_remove
#define dyna_set        zd_dyna_set
#define dyna_get        zd_dyna_get
#define dyna_free       zd_dyna_free
#define dyna_next       zd_dyna_next

/* MODULE: ZD_DS_STRING */

struct zd_string {
    char *base;
    size_t length;
    size_t capacity;
};

ZD_DEF void zd_string_append(struct zd_string *str, const char *fmt, ...);
ZD_DEF struct zd_string zd_string_sub(const char *str, size_t src, size_t dest);
ZD_DEF struct zd_string zd_string_repeat(const char *str, size_t times);
ZD_DEF struct zd_string zd_string_replace(const char *str,
        const char *s_old, const char *s_new);
ZD_DEF void zd_string_free(void *arg);
static struct zd_string _string_concat(const char *delim, ...);

#ifndef zd_string_concat
  #ifdef ZD_IMPLEMENTATION
    #define zd_string_concat(delim, ...) _string_concat(delim, ##__VA_ARGS__, NULL)
  #else
    #define zd_string_concat(delim, ...)
  #endif
#endif

typedef struct zd_string string_t;

#define string_append   zd_string_append
#define string_concat   zd_string_concat
#define string_sub      zd_string_sub
#define string_repeat   zd_string_repeat
#define string_replace  zd_string_replace
#define string_free     zd_string_free

/* MODULE: ZD_DS_STACK */

struct zd_stack {
    void *base;
    int top;
    size_t capacity;
    void (*clear_item)(void *);
    size_t size;    /* size of each element */
    struct zd_dyna gc;
};

ZD_DEF void zd_stack_init(struct zd_stack *stk, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_stack_push(struct zd_stack *stk, void *elem);
ZD_DEF void *zd_stack_pop(struct zd_stack *stk);
ZD_DEF void *zd_stack_top(struct zd_stack *stk);
ZD_DEF void zd_stack_free(struct zd_stack *stk);

typedef struct zd_stack stack_t;

#define stack_init      zd_stack_init
#define stack_push      zd_stack_push
#define stack_pop       zd_stack_pop
#define stack_top       zd_stack_top
#define stack_free      zd_stack_free

/* MODULE: ZD_DS_QUEUE */

struct zd_queue_node {
    void *data;
    struct zd_queue_node *next;
};

struct zd_queue {
    struct zd_queue_node head;  /* dummy */
    struct zd_queue_node *front, *rear;
    size_t count;
    size_t size;    /* size of each element */
    struct zd_dyna gc;
    void (*clear_item)(void *);
};

#ifndef zd_queue_isempty
  #ifdef ZD_IMPLEMENTATION
    #define zd_queue_isempty(qp) ((qp)->head.next == NULL)
  #else
    #define zd_queue_isempty(qp)
  #endif
#endif

ZD_DEF void zd_queue_init(struct zd_queue *qp, size_t size,
        void (*clear_item)(void *));
ZD_DEF void zd_queue_free(struct zd_queue *qp);
ZD_DEF void *zd_queue_front(struct zd_queue *qp);
ZD_DEF void *zd_queue_rear(struct zd_queue *qp);
ZD_DEF void zd_queue_push(struct zd_queue *qp, void *elem);
ZD_DEF void *zd_queue_pop(struct zd_queue *qp);

typedef struct zd_queue_node queue_node_t;
typedef struct zd_queue queue_t;

#define queue_init      zd_queue_init
#define queue_free      zd_queue_free
#define queue_front     zd_queue_front
#define queue_rear      zd_queue_rear
#define queue_push      zd_queue_push
#define queue_pop       zd_queue_pop
#define queue_isempty   zd_queue_isempty

/* MODULE: ZD_FS */

#ifdef _WIN32
  #include <windows.h>
  #include <direct.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <io.h>
  #include <sys/stat.h>
#else
  #include <unistd.h>
  #include <dirent.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <time.h>
  #include <sys/stat.h>
  #include <sys/types.h>
#endif /* platform */

#define FT_NOET 0    /* not exist */
#define FT_REG  1    /* regular file */
#define FT_DIR  2    /* directory */

#define FA_NONE  0
#define FA_EXEC  (1 << 0)
#define FA_READ  (1 << 1)
#define FA_WRITE (1 << 2)

#define FS_MAX_PATH_SIZE 1024

struct zd_meta_file {
    char *name;
    char *content;
    int type;
    int attr;
    size_t size;
    size_t line;
};

struct zd_meta_dir {
    char *name;
    char **files;
    char **dirs;
    size_t f_cnt;
    size_t d_cnt;
    size_t count;
};

ZD_DEF unsigned long long zd_fs_get_timestamp(const char *pathname);
ZD_DEF char *zd_fs_get_name(const char *pathname);
ZD_DEF bool zd_fs_pwd(char *buf, size_t buf_size);
ZD_DEF bool zd_fs_cd(const char *pathname);
ZD_DEF bool zd_fs_move(const char *src, const char *dest, bool is_binary);
ZD_DEF bool zd_fs_copy(const char *src, const char *dest, bool is_binary);
ZD_DEF bool zd_fs_mkdir(const char *pathname);
ZD_DEF bool zd_fs_touch(const char *pathname);
ZD_DEF bool zd_fs_remove(const char *pathname);
ZD_DEF bool zd_fs_remove_all(struct zd_dyna *items);
ZD_DEF struct zd_dyna zd_fs_match(const char *pathname, const char *pattern);
ZD_DEF struct zd_dyna zd_fs_find(const char *pathname, int attrs);
ZD_DEF int zd_fs_typeof(const char *pathname);
ZD_DEF bool zd_fs_loadf(const char *filename,
        struct zd_meta_file *res, bool is_binary);
ZD_DEF bool zd_fs_loadd(const char *dirname, struct zd_meta_dir *res);
ZD_DEF bool zd_fs_dumpf(const char *dest_file,
        const char *src_buf, size_t size, bool is_binary);
ZD_DEF void zd_fs_free_mf(struct zd_meta_file *mf);
ZD_DEF void zd_fs_free_md(struct zd_meta_dir *md);
ZD_DEF int zd_fs_get_attr(const char *filename);
ZD_DEF bool zd_fs_check_perm(const char *filename, int perm);

typedef struct zd_meta_file metafile_t;
typedef struct zd_meta_dir  metadir_t;

#define fs_get_timestamp    zd_fs_get_timestamp
#define fs_get_name         zd_fs_get_name
#define fs_get_attr         zd_fs_get_attr
#define fs_check_perm       zd_fs_check_perm
#define fs_pwd              zd_fs_pwd
#define fs_cd               zd_fs_cd
#define fs_move             zd_fs_move
#define fs_copy             zd_fs_copy
#define fs_mkdir            zd_fs_mkdir
#define fs_touch            zd_fs_touch
#define fs_remove           zd_fs_remove
#define fs_remove_all       zd_fs_remove_all
#define fs_match            zd_fs_match
#define fs_find             zd_fs_find
#define fs_typeof           zd_fs_typeof
#define fs_loadf            zd_fs_loadf
#define fs_loadd            zd_fs_loadd
#define fs_dumpf            zd_fs_dumpf
#define fs_free_mf          zd_fs_free_mf
#define fs_free_md          zd_fs_free_md

/* MODULE: ZD_COMMAND_LINE */

/* option type */
#define OPTT_NO_ARG     0
#define OPTT_SINGLE_ARG 1
#define OPTT_MULTI_ARG  2

struct zd_cmdlopt {
    int type;
    bool is_defined;        /* is it a rule */
    struct zd_string name;  /* used in command line */
    struct zd_dyna vals;    /* target values, each element is 'struct zd_string' */
    struct zd_dyna pargs;   /* positional args, each element is 'struct zd_string' */

    /* for rule */
    struct zd_string lname; /* long name (use --) */
    struct zd_string sname; /* short name (use -) */
    struct zd_string description;
};

struct zd_cmdl {
    struct zd_string program;
    struct zd_dyna pargs;   /* positional args, each element is 'struct zd_string' */
    struct zd_dyna opts;    /* each element is 'struct zd_cmdlopt' */
    struct zd_dyna rules;   /* each element is 'struct zd_cmdlopt' */

    /* config */
    bool merge_opt;     /* merge options if they are the same */
};

ZD_DEF void zd_cmdl_init(struct zd_cmdl *cmdl, bool merge_opt);
ZD_DEF void zd_cmdl_define(struct zd_cmdl *cmdl, int type,
        const char *lname, const char *sname, const char *description);
ZD_DEF void zd_cmdl_build(struct zd_cmdl *cmdl, int argc, char **argv);
ZD_DEF void zd_cmdl_usage(struct zd_cmdl *cmdl);
ZD_DEF bool zd_cmdl_isuse(struct zd_cmdl *cmdl, const char *optname);
ZD_DEF struct zd_cmdlopt *zd_cmdl_get_opt(struct zd_cmdl *cmdl, 
        const char *optname);
ZD_DEF void zd_cmdl_free(void *arg);
ZD_DEF void zd_cmdl_dump(struct zd_cmdl *cmdl);
ZD_DEF void zd_cmdlopt_init(struct zd_cmdlopt *opt);
ZD_DEF void zd_cmdlopt_dump(struct zd_cmdlopt *opt, size_t level);
ZD_DEF void zd_cmdlopt_free(void *arg);

typedef struct zd_cmdlopt   cmdlopt_t;
typedef struct zd_cmdl      cmdl_t;

#define cmdl_init       zd_cmdl_init
#define cmdl_define     zd_cmdl_define
#define cmdl_build      zd_cmdl_build
#define cmdl_usage      zd_cmdl_usage
#define cmdl_isuse      zd_cmdl_isuse
#define cmdl_getopt     zd_cmdl_get_opt
#define cmdl_free       zd_cmdl_free
#define cmdl_dump       zd_cmdl_dump
#define cmdlopt_init    zd_cmdlopt_init
#define cmdlopt_dump    zd_cmdlopt_dump
#define cmdlopt_free    zd_cmdlopt_free

/* MODULE: ZD_BUILD */

#define EXEC_UNDO   0
#define EXEC_OK     1
#define EXEC_ERROR  2

struct zd_cmd {
    struct zd_dyna args;    /* each element is struct zd_string */
    size_t count;
};

struct zd_builder {
    struct zd_dyna cmds;    /* each element is struct zd_cmd */
    size_t count;
};

static void _cmd_append_arg(struct zd_cmd *cmd, ...);
ZD_DEF void zd_cmd_init(struct zd_cmd *cmd);
ZD_DEF void zd_cmd_print(struct zd_cmd *cmd);
ZD_DEF int zd_cmd_run(struct zd_cmd *cmd);
ZD_DEF void zd_cmd_free(struct zd_cmd *cmd);

#ifndef zd_cmd_append_arg
  #ifdef ZD_IMPLEMENTATION
    #define zd_cmd_append_arg(cmd, ...) \
        _cmd_append_arg((cmd), __VA_ARGS__, NULL)
  #else
    #define zd_cmd_append_arg(cmd, ...)
  #endif
#endif

static void _build_append_cmd(struct zd_builder *builder, ...);
static void _build_update_self(const char *cc, int argc,
        char **argv, const char *source, ...);
ZD_DEF void zd_build_init(struct zd_builder *builder);
ZD_DEF void zd_build_free(struct zd_builder *builder);
ZD_DEF void zd_build_print(struct zd_builder *builder);
ZD_DEF int zd_build_run_sync(struct zd_builder *builder);

#ifndef zd_build_update_self
  #ifdef ZD_IMPLEMENTATION
    #define zd_build_update_self(cc, argc, argv, ...) \
        _build_update_self(cc, argc, argv, __FILE__, ##__VA_ARGS__, NULL)
  #else
    #define zd_build_update_self
  #endif
#endif

#ifndef zd_build_append_cmd
  #ifdef ZD_IMPLEMENTATION
    #define zd_build_append_cmd(builder, ...) \
        _build_append_cmd((builder), ##__VA_ARGS__, NULL)
  #else
    #define zd_build_append_cmd
  #endif
#endif

typedef struct zd_cmd       cmd_t;
typedef struct zd_builder   builder_t;

#define cmd_append_arg      zd_cmd_append_arg
#define cmd_init            zd_cmd_init
#define cmd_print           zd_cmd_print
#define cmd_run             zd_cmd_run
#define cmd_free            zd_cmd_free
#define build_append_cmd    zd_build_append_cmd
#define build_init          zd_build_init
#define build_free          zd_build_free
#define build_print         zd_build_print
#define build_run_sync      zd_build_run_sync
#define build_update_self   zd_build_update_self

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ZD_H */

#ifdef ZD_IMPLEMENTATION

/* MODULE: ZD_COMMAND_LINE */

ZD_DEF void zd_cmdl_dump(struct zd_cmdl *cmdl)
{
    fprintf(stderr, "<program>: %s\n", cmdl->program.base);

    if (cmdl->pargs.count > 0) {
        fprintf(stderr, "<pargs>:\n");
        for (size_t i = 0; i < cmdl->pargs.count; i++) {
            struct zd_string *arg = zd_dyna_get(&cmdl->pargs, i);
            fprintf(stderr, "\tpargs[%zu]: %s\n", i, arg->base);
        }
    } else {
        fprintf(stderr, "<pargs>: EMPTY\n");
    }

    if (cmdl->opts.count > 0) {
        fprintf(stderr, "<opts>:\n");
        for (size_t i = 0; i < cmdl->opts.count; i++) {
            fprintf(stderr, "\topts[%zu]:\n", i);
            struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->opts, i);
            zd_cmdlopt_dump(opt, 2);
        }
    } else {
        fprintf(stderr, "<opts>: EMPTY\n");
    }
}

static inline void _optt_to_string(int type, char *buf, size_t n)
{
    switch (type) {
    case OPTT_NO_ARG:
        snprintf(buf, n, "NO ARG");
        break;
    case OPTT_SINGLE_ARG:
        snprintf(buf, n, "SINGLE ARG");
        break;
    case OPTT_MULTI_ARG:
        snprintf(buf, n, "MULTI ARG");
        break;
    default:
        snprintf(buf, n, "UNKNOWN");
        break;
    }
}

ZD_DEF void zd_cmdlopt_dump(struct zd_cmdlopt *opt, size_t level)
{
    struct zd_string indent = zd_string_repeat("\t", level);

    char type_str[20];
    _optt_to_string(opt->type, type_str, sizeof(type_str));

    fprintf(stderr, "%s<name>: %s\n", indent.base, opt->name.base);
    fprintf(stderr, "%s<type>: %s\n", indent.base, type_str);
    fprintf(stderr, "%s<rule>: %s\n", indent.base, opt->is_defined ? "true" : "false");

    if (opt->vals.count > 0) {
        fprintf(stderr, "%s<vals>:\n", indent.base);
        for (size_t i = 0; i < opt->vals.count; i++) {
            struct zd_string *val = zd_dyna_get(&opt->vals, i);
            fprintf(stderr, "%s\tvals[%zu]: %s\n", indent.base, i, val->base);
        }
    } else {
        fprintf(stderr, "%s<vals>: EMPTY\n", indent.base);
    }

    if (opt->pargs.count > 0) {
        fprintf(stderr, "%s<pargs>:\n", indent.base);
        for (size_t i = 0; i < opt->pargs.count; i++) {
            struct zd_string *arg = zd_dyna_get(&opt->pargs, i);
            fprintf(stderr, "%s\tpargs[%zu]: %s\n", indent.base, i, arg->base);
        }
    } else {
        fprintf(stderr, "%s<pargs>: EMPTY\n", indent.base);
    }

    zd_string_free(&indent);
}

ZD_DEF void zd_cmdlopt_init(struct zd_cmdlopt *opt)
{
    opt->type = OPTT_NO_ARG;
    opt->is_defined = false;
    opt->name = (struct zd_string) {0};
    opt->lname = (struct zd_string) {0};
    opt->sname = (struct zd_string) {0};
    zd_dyna_init(&opt->vals, sizeof(struct zd_string), zd_string_free);
    zd_dyna_init(&opt->pargs, sizeof(struct zd_string), zd_string_free);
    opt->description = (struct zd_string) {0};
}

ZD_DEF void zd_cmdl_init(struct zd_cmdl *cmdl, bool merge_opt)
{
    cmdl->program = (struct zd_string) {0};
    zd_dyna_init(&cmdl->pargs, sizeof(struct zd_string), zd_string_free);
    zd_dyna_init(&cmdl->opts, sizeof(struct zd_cmdlopt), zd_cmdlopt_free);
    zd_dyna_init(&cmdl->rules, sizeof(struct zd_cmdlopt), zd_cmdlopt_free);
    cmdl->merge_opt = merge_opt;
}

ZD_DEF void zd_cmdl_define(struct zd_cmdl *cmdl, int type,
        const char *lname, const char *sname, const char *description)
{
    if ((type < 0 || type > 2) || (!lname && !sname))
        return;

    struct zd_cmdlopt opt = {0};
    zd_cmdlopt_init(&opt);

    opt.type = type;
    zd_string_append(&opt.lname, lname ? lname : "@@");
    zd_string_append(&opt.sname, sname ? sname : "@@");
    zd_string_append(&opt.description, description ? description : "@@");

    zd_dyna_append(&cmdl->rules, &opt);
}

static inline char *_skip_dash(char *arg)
{
    for (int i = 0; i < (int) strlen(arg); i++) {
        if (arg[i] == '-')
            continue;
        return arg + i;
    }

    return NULL;
}

static inline void _check_opt(const char *optname,
        bool one_dash, bool is_short)
{
    if (is_short != one_dash) {
        if (one_dash) {
            zd_log(LOG_FATAL, "'-%s' is invalid", optname);
        } else {
            zd_log(LOG_FATAL, "'--%s' is invalid", optname);
        }
    }
}

static inline struct zd_cmdlopt *_get_rule(struct zd_cmdl *cmdl,
        const char *optname, bool *is_short)
{
    struct zd_string s;
    struct zd_cmdlopt *best_match = NULL;
    size_t max_match_len = 0;

    for (size_t i = 0; i < cmdl->rules.count; i++) {
        struct zd_cmdlopt *rule = zd_dyna_get(&cmdl->rules, i);

        s = (struct zd_string) {0};
        zd_string_append(&s, "%s*", rule->lname.base);
        if (zd_wildcard_match(optname, s.base)) {
            size_t match_len = strlen(rule->lname.base);
            if (match_len > max_match_len) {
                max_match_len = match_len;
                best_match = rule;
                if (is_short) *is_short = false;
            }
        }
        zd_string_free(&s);

        s = (struct zd_string) {0};
        zd_string_append(&s, "%s*", rule->sname.base);
        if (zd_wildcard_match(optname, s.base)) {
            size_t match_len = strlen(rule->sname.base);
            if (match_len > max_match_len) {
                max_match_len = match_len;
                best_match = rule;
                if (is_short) *is_short = true;
            }
        }
        zd_string_free(&s);
    }

    return best_match;
}

static inline bool is_dyna_value_exist(struct zd_dyna *da,
        struct zd_string *val)
{
    for (size_t i = 0; i < da->count; i++) {
        struct zd_string *s = zd_dyna_get(da, i);
        if (strcmp(s->base, val->base) == 0)
            return true;
    }
    return false;
}

ZD_DEF void zd_cmdl_build(struct zd_cmdl *cmdl, int argc, char **argv)
{
    int pos = 0;     /* track the arg */

    zd_string_append(&cmdl->program, argv[pos++]);

    /* handle the positional args for program without options */
    while (pos < argc) {
        if (argv[pos][0] == '-')
            break;
        struct zd_string arg = {0};
        zd_string_append(&arg, argv[pos]);
        zd_dyna_append(&cmdl->pargs, &arg);
        pos++;
    }

    /* handle the remaining options */
    while (pos < argc) {
        bool one_dash = false;
        if (argv[pos][0] == '-' && argv[pos][1] == '-')
            one_dash = false;
        else
            one_dash = true;

        const char *p1 = _skip_dash(argv[pos]);
        if (!p1) {
            pos++;
            continue;
        }
        const char *p2 = strchr(p1, '=');
        bool has_equal = (p2 != NULL);


        struct zd_cmdlopt opt = {0};
        zd_cmdlopt_init(&opt);

        bool is_short = false;
        struct zd_cmdlopt *rule = _get_rule(cmdl, p1, &is_short);
        if (rule)
            _check_opt(p1, one_dash, is_short);

        /* add target values for this option according to its type */
        if (has_equal) {
            if (rule) {
                opt.is_defined = true;
                opt.type = rule->type;
                if (!is_short)
                    zd_string_append(&opt.name, rule->lname.base);
                else
                    zd_string_append(&opt.name, rule->sname.base);
            } else {
                opt.name = zd_string_sub(p1, 0, p2 - p1);
                opt.is_defined = false;
                opt.type = OPTT_SINGLE_ARG;
            }

            struct zd_string val = {0};
            if (*(p2 + 1)) {
                zd_string_append(&val, p2 + 1);
                zd_dyna_append(&opt.vals, &val);
            }

            pos++;
        } else { 
            if (rule) {
                opt.is_defined = true;
                opt.type = rule->type;
                if (!is_short)
                    zd_string_append(&opt.name, rule->lname.base);
                else
                    zd_string_append(&opt.name, rule->sname.base);

                p2 = p1 + opt.name.length;

                struct zd_string val = {0};
                if (*p2) {
                    zd_string_append(&val, p2);
                    zd_dyna_append(&opt.vals, &val);
                }
            } else {
                zd_string_append(&opt.name, p1);
                opt.is_defined = false;
                opt.type = OPTT_MULTI_ARG;
            }

            pos++;

            struct zd_string val;
            switch (opt.type) {
            case OPTT_NO_ARG:
                break;

            case OPTT_SINGLE_ARG:
                if (opt.vals.count == 1)
                    break;
                if (pos >= argc)
                    zd_log(LOG_FATAL, "option '%s' should receive one argument",
                            opt.name.base);
                if (argv[pos][0] == '-')
                    zd_log(LOG_FATAL, "'%s' should be an argument of option '%s'",
                            argv[pos], opt.name.base);
                val = (struct zd_string) {0};
                zd_string_append(&val, argv[pos]);
                zd_dyna_append(&opt.vals, &val);
                pos++;
                break;

            case OPTT_MULTI_ARG:
                while (pos < argc) {
                    if (argv[pos][0] == '-')
                        break;
                    val = (struct zd_string) {0};
                    zd_string_append(&val, argv[pos]);
                    zd_dyna_append(&opt.vals, &val);
                    pos++;
                }
                break;

            default:
                zd_log(LOG_FATAL, "invalid option type");
                break;
            }
        }

        /* add positional args for this option */
        while (pos < argc) {
            if (argv[pos][0] == '-')
                break;
            struct zd_string arg = {0};
            zd_string_append(&arg, argv[pos]);
            zd_dyna_append(&opt.pargs, &arg);
            pos++;
        }

        if (cmdl->merge_opt) {
            /* append all stuff if the option had been used instead of adding */
            bool is_use = zd_cmdl_isuse(cmdl, opt.name.base);
            if (is_use) {
                struct zd_cmdlopt *saved_opt;
                for (size_t i = 0; i < cmdl->opts.count; i++) {
                    saved_opt = zd_dyna_get(&cmdl->opts, i);
                    if (strcmp(saved_opt->name.base, opt.name.base) == 0)
                        break;
                }

                struct zd_string *val;
                for (size_t i = 0; i < opt.vals.count; i++) {
                    val = zd_dyna_get(&opt.vals, i);
                    if (is_dyna_value_exist(&saved_opt->vals, val))
                        continue;
                    zd_dyna_append(&saved_opt->vals, val);
                }
                struct zd_string *arg;
                for (size_t i = 0; i < opt.pargs.count; i++) {
                    arg = zd_dyna_get(&opt.pargs, i);
                    if (is_dyna_value_exist(&saved_opt->pargs, arg))
                        continue;
                    zd_dyna_append(&saved_opt->pargs, arg);
                }

                if (opt.vals.base)
                    free(opt.vals.base);
                if (opt.pargs.base)
                    free(opt.pargs.base);
                zd_string_free(&opt.name);
            } else {
                zd_dyna_append(&cmdl->opts, &opt);
            }
        } else {
            /* directly append it */
            zd_dyna_append(&cmdl->opts, &opt);
        }
    }
}

ZD_DEF void zd_cmdl_usage(struct zd_cmdl *cmdl)
{
    fprintf(stderr, "Usage: %s ...\n", cmdl->program.base);
    for (size_t i = 0; i < cmdl->rules.count; i++) {
        struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->rules, i);
        struct zd_string name = {0};
        zd_string_append(&name, "-%s --%s", opt->sname.base, opt->lname.base);
        fprintf(stderr, "  %-20s %s\n", name.base, opt->description.base);
        zd_string_free(&name);
    }
}

ZD_DEF bool zd_cmdl_isuse(struct zd_cmdl *cmdl, const char *optname)
{
    if (!optname)
        return false;

    for (size_t i = 0; i < cmdl->opts.count; i++) {
        struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->opts, i);
        if (opt->is_defined) {
            struct zd_cmdlopt *rule = _get_rule(cmdl, opt->name.base, NULL);
            if ((rule->lname.base && strcmp(rule->lname.base, optname) == 0) ||
                (rule->sname.base && strcmp(rule->sname.base, optname) == 0))
                return true;
        } else {
            if (opt->name.base && strcmp(opt->name.base, optname) == 0)
                return true;
        }
    }
    return false;
}

ZD_DEF struct zd_cmdlopt *zd_cmdl_get_opt(struct zd_cmdl *cmdl, 
        const char *optname)
{
    if (!optname)
        return false;

    for (size_t i = 0; i < cmdl->opts.count; i++) {
        struct zd_cmdlopt *opt = zd_dyna_get(&cmdl->opts, i);
        if (opt->is_defined) {
            struct zd_cmdlopt *rule = _get_rule(cmdl, opt->name.base, NULL);
            if ((rule->lname.base && strcmp(rule->lname.base, optname) == 0) ||
                (rule->sname.base && strcmp(rule->sname.base, optname) == 0))
                return opt;
        } else {
            if (opt->name.base && strcmp(opt->name.base, optname) == 0)
                return opt;
        }
    }

    return NULL;
}

ZD_DEF void zd_cmdl_free(void *arg)
{
    struct zd_cmdl *cmdl = (struct zd_cmdl *) arg;
    zd_string_free(&cmdl->program);
    zd_dyna_free(&cmdl->pargs);
    zd_dyna_free(&cmdl->opts);
    zd_dyna_free(&cmdl->rules);
    cmdl->merge_opt = false;
}

ZD_DEF void zd_cmdlopt_free(void *arg)
{
    struct zd_cmdlopt *opt = (struct zd_cmdlopt *) arg;
    opt->type = OPTT_NO_ARG;
    opt->is_defined = false;
    zd_string_free(&opt->name);
    zd_string_free(&opt->lname);
    zd_string_free(&opt->sname);
    zd_dyna_free(&opt->vals);
    zd_dyna_free(&opt->pargs);
    zd_string_free(&opt->description);
}

/* MODULE: ZD_FS */

static inline int count_line(char *buf, size_t size)
{
    if (!buf)
        return 0;

    int count = 0;
    for (size_t i = 0; i < size; i++)
        if (buf[i] == '\n')
            count++;

    if (size > 0 && buf[size - 1] != '\n')
        count++;

    return count;
}

/* return zero if error */
ZD_DEF unsigned long long zd_fs_get_timestamp(const char *pathname)
{
    unsigned long long timestamp = 0;

#ifdef _WIN32
    HANDLE hFile = CreateFile(pathname, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR: unable to open file\n");
        return timestamp;
    }

    FILETIME ftCreate, ftAccess, ftWrite;
    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
        
        ULONGLONG ft = (((ULONGLONG) ftWrite.dwHighDateTime) << 32) + ftWrite.dwLowDateTime;
        timestamp = (unsigned long long)((ft - 116444736000000000LL) / 10000000);
    } else {
        fprintf(stderr, "ERROR: failed to get file time\n");
    }
    CloseHandle(hFile);
#else
    struct stat sb;
    if (stat(pathname, &sb) == -1) {
        fprintf(stderr, "ERROR: unable to get file stats");
        return timestamp;
    }

    timestamp = (unsigned long long) sb.st_mtime;
#endif /* platform */

    return timestamp;
}

ZD_DEF char *zd_fs_get_name(const char *pathname)
{
    if (!pathname)
        return NULL;

#ifdef _WIN32
    char *filename = strrchr(pathname, '\\');
#else
    char *filename = strrchr(pathname, '/');
#endif /* platform */

    if (filename)
        return filename + 1;
    return (char *) pathname;
}

ZD_DEF bool zd_fs_pwd(char *buf, size_t buf_size)
{
    if (!buf || buf_size == 0)
        return false;

#ifdef _WIN32
    if (GetCurrentDirectory(buf_size, buf))
        return true;
    else
        return false;
#else
    if (getcwd(buf, buf_size))
        return true;
    else
        return false;
#endif /* platform */
}

ZD_DEF bool zd_fs_cd(const char *pathname)
{
    if (!pathname)
        return false;

#ifdef _WIN32
    if (SetCurrentDirectory(pathname))
        return true;
    else
        return false;
#else
    if (chdir(pathname) == 0)
        return true;
    else
        return false;
#endif /* platform */
}

ZD_DEF bool zd_fs_move(const char *src, const char *dest, bool is_binary)
{
    if (zd_fs_typeof(src) != FT_REG)
        return false;

    if (!zd_fs_copy(src, dest, is_binary) || !zd_fs_remove(src))
        return false;
    return true;
}

ZD_DEF bool zd_fs_remove_all(struct zd_dyna *items)
{
    bool status = true;
    for (size_t i = 0; i < items->count; i++) {
        struct zd_string *file = zd_dyna_get(items, i);
        status &= zd_fs_remove(file->base);
    }
    return status;
}

ZD_DEF bool zd_fs_copy(const char *src, const char *dest, bool is_binary)
{
    if (zd_fs_typeof(src) != FT_REG)
        return false;

    char dest_path[FS_MAX_PATH_SIZE];
    if (zd_fs_typeof(dest) == FT_NOET) {
        if (dest[strlen(dest) - 1] == '\\' || dest[strlen(dest) - 1] == '/')
            zd_fs_mkdir(dest);
    }

    if (zd_fs_typeof(dest) == FT_DIR)
        snprintf(dest_path, sizeof(dest_path), "%s/%s",
                dest, zd_fs_get_name(src));
    else
        snprintf(dest_path, sizeof(dest_path), "%s", dest);

    struct zd_meta_file mf = {0};
    if (!zd_fs_loadf(src, &mf, is_binary))
        return false;
    if (!zd_fs_dumpf(dest_path, mf.content, mf.size, is_binary)) {
        zd_fs_free_mf(&mf);
        return false;
    }
    zd_fs_free_mf(&mf);
    return true;
}

ZD_DEF bool zd_fs_mkdir(const char *pathname)
{
    char path_copy[FS_MAX_PATH_SIZE];
    strncpy(path_copy, pathname, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    /* process step by step in the path string */
    char *dir = path_copy;

#ifdef _WIN32
    while ((dir = strchr(dir, '\\')) != NULL) {
        /* replace the path separator with a string terminator */
        *dir = '\0';
        /* create this directory */
        if (path_copy[0] != '\0' && _mkdir(path_copy) != 0 && errno != EEXIST)
            return false;
        /* restore the seperator and process next path */
        *dir = '\\';
        dir++;
    }
    /* create final directory */
    return (_mkdir(pathname) == 0 || errno == EEXIST);
#else
    while ((dir = strchr(dir, '/')) != NULL) {
        /* replace the path separator with a string terminator */
        *dir = '\0';
        /* create this directory */
        if (path_copy[0] != '\0' && /* avoid path_copy to be "" in case /home/... */
                mkdir(path_copy, S_IRWXU | S_IRWXG | S_IRWXO) != 0 &&
                errno != EEXIST)
            return false;
        /* restore the seperator and process next path */
        *dir = '/';
        dir++;
    }
    return (mkdir(pathname, S_IRWXU | S_IRWXG | S_IRWXO) == 0 ||
            errno == EEXIST);
#endif /* platform */
}

ZD_DEF bool zd_fs_touch(const char *pathname)
{
    FILE *fp = fopen(pathname, "a");
    if (fp == NULL)
        return false;
    fclose(fp);
    return true;
}

static bool _remove_dir_recursively(const char *dirpath)
{
#ifdef _WIN32
    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = INVALID_HANDLE_VALUE;

    char dir[FS_MAX_PATH_SIZE];
    snprintf(dir, sizeof(dir), "%s\\*", dirpath);

    h_find = FindFirstFile(dir, &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE)
        return false;

    do {
        if (strcmp(find_file_data.cFileName, ".") != 0 &&
                strcmp(find_file_data.cFileName, "..") != 0) {
            char filepath[FS_MAX_PATH_SIZE];
            snprintf(filepath, sizeof(filepath), "%s\\%s",
                    dirpath, find_file_data.cFileName);

            int type = zd_fs_typeof(filepath);
            if (type == FT_NOET)
                break;

            if (type == FT_DIR)
                _remove_dir_recursively(filepath);
            else
                DeleteFile(filepath); 
        }
    } while (FindNextFile(h_find, &find_file_data) != 0);

    FindClose(h_find);
    return RemoveDirectory(dirpath) != 0;
#else
    DIR *dp = opendir(dirpath);
    if (!dp)
        return false;

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
                strcmp(entry->d_name, "..") != 0) {
            char filepath[FS_MAX_PATH_SIZE];
            snprintf(filepath, sizeof(filepath), "%s/%s",
                    dirpath, entry->d_name);

            int type = zd_fs_typeof(filepath);
            if (type == FT_NOET)
                break;

            if (type == FT_DIR) {
                if (!_remove_dir_recursively(filepath)) {
                    closedir(dp);
                    return false;
                }
            } else {
                if (unlink(filepath) != 0) {
                    closedir(dp);
                    return false;
                }
            }
        }
    }
    closedir(dp);
    return rmdir(dirpath) == 0;
#endif /* platform */
}

ZD_DEF struct zd_dyna zd_fs_match(const char *pathname, const char *pattern)
{
    struct zd_dyna res = {0};
    zd_dyna_init(&res, sizeof(struct zd_string), zd_string_free);

    if (!pathname || !pattern) return res;

    int type = zd_fs_typeof(pathname);
    if (type == FT_DIR) {
        struct zd_meta_dir md = {0};
        zd_fs_loadd(pathname, &md);
        for (size_t i = 0; i < md.f_cnt; i++) {
            if (wc_match(md.files[i], pattern)) {
                struct zd_string file = {0};
                zd_string_append(&file, md.files[i]); 
                zd_dyna_append(&res, &file);
            }
        }
        zd_fs_free_md(&md);
    } else if (type == FT_REG) {
        if (wc_match(pathname, pattern)) {
            struct zd_string file = {0};
            zd_string_append(&file, pathname);
            zd_dyna_append(&res, &file);
        }
    } 

    return res;
}

ZD_DEF struct zd_dyna zd_fs_find(const char *pathname, int attrs)
{
    struct zd_dyna res = {0};
    zd_dyna_init(&res, sizeof(struct zd_string), zd_string_free);

    if (!pathname) return res;

    int type = zd_fs_typeof(pathname);
    if (type == FT_DIR) {
        struct zd_meta_dir md = {0};
        zd_fs_loadd(pathname, &md);
        for (size_t i = 0; i < md.f_cnt; i++) {
            if (zd_fs_check_perm(md.files[i], attrs)) {
                struct zd_string file = {0};
                zd_string_append(&file, md.files[i]); 
                zd_dyna_append(&res, &file);
            }
        }
        zd_fs_free_md(&md);
    } else if (type == FT_REG) {
        if (zd_fs_check_perm(pathname, attrs)) {
            struct zd_string file = {0};
            zd_string_append(&file, pathname);
            zd_dyna_append(&res, &file);
        }
    } 

    return res;
}

ZD_DEF bool zd_fs_remove(const char *pathname)
{
    int type = zd_fs_typeof(pathname);
    if (type == FT_NOET) 
        return true;
    else if (type == FT_DIR)
        return _remove_dir_recursively(pathname);
    else
        return remove(pathname) == 0;
}

ZD_DEF int zd_fs_typeof(const char *pathname)
{
    if (!pathname)
        return FT_NOET;

#ifdef _WIN32
    DWORD attr = GetFileAttributesA(pathname);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return FT_NOET;

    if (attr & FILE_ATTRIBUTE_DIRECTORY)
        return FT_DIR;
    else
        return FT_REG;
#else
    struct stat sb;
    if (stat(pathname, &sb) == -1) 
        return FT_NOET;

    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:
    case S_IFCHR:
    case S_IFIFO:
    case S_IFLNK:
    case S_IFSOCK:
    case S_IFREG:
        return FT_REG;
    case S_IFDIR:
        return FT_DIR;
    default: 
        return FT_NOET;
    }
#endif /* platform */
}

ZD_DEF bool zd_fs_loadd(const char *dirname, struct zd_meta_dir *res)
{
    if (!dirname)
        return false;

    if (zd_fs_typeof(dirname) != FT_DIR)
        return false;

    res->name = strdup(dirname);
    assert(res->name != NULL);


#ifdef _WIN32
    bool has_slash = dirname[strlen(dirname) - 1] == '\\';

    char buf[FS_MAX_PATH_SIZE];
    if (!has_slash) snprintf(buf, sizeof(buf), "%s\\*", dirname);
    else snprintf(buf, sizeof(buf), "%s*", dirname);
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(buf, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        free(res->name);
        res->name = NULL;
        return false;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 ||
                strcmp(findFileData.cFileName, "..") == 0)
            continue;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            res->dirs = realloc(res->dirs, sizeof(char *) * (res->d_cnt + 1));
            assert(res->dirs != NULL);
            res->dirs[res->d_cnt] = strdup(findFileData.cFileName);
            assert(res->dirs[res->d_cnt] != NULL);
            res->d_cnt++;
        } else {
            res->files = realloc(res->files, sizeof(char *) * (res->f_cnt + 1));
            assert(res->files != NULL);
            res->files[res->f_cnt] = strdup(findFileData.cFileName);
            assert(res->files[res->f_cnt] != NULL);
            res->f_cnt++;
        }

        res->count++;
    } while (FindNextFile(hFind, &findFileData));

    FindClose(hFind);
    return true;
#else
    bool has_slash = dirname[strlen(dirname) - 1] == '/';

    DIR *dp = opendir(dirname);
    if (!dp) {
        free(res->name);
        res->name = NULL;
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t len = strlen(dirname) + strlen(entry->d_name) + 2;
        char *full_path = (char *) malloc(len);
        if (!has_slash) snprintf(full_path, len, "%s/%s", dirname, entry->d_name);
        else snprintf(full_path, len, "%s%s", dirname, entry->d_name);

        int type = zd_fs_typeof(full_path);
        if (type == FT_REG) {
            res->files = realloc(res->files, sizeof(char *) * (res->f_cnt + 1));
            assert(res->files != NULL);
            res->files[res->f_cnt] = strdup(full_path);
            assert(res->files[res->f_cnt] != NULL);
            res->f_cnt++;
        } else {
            res->dirs = realloc(res->dirs, sizeof(char *) * (res->d_cnt + 1));
            assert(res->dirs != NULL);
            res->dirs[res->d_cnt] = strdup(full_path);
            assert(res->dirs[res->d_cnt] != NULL);
            res->d_cnt++;
        }

        free(full_path);
        res->count++;
    }

    closedir(dp);
    return true;
#endif /* platform */
}

ZD_DEF bool zd_fs_loadf(const char *filename,
        struct zd_meta_file *res, bool is_binary)
{
    if (!filename)
        return false;

    res->type = zd_fs_typeof(filename);
    if (res->type != FT_REG)
        return false;

    res->name = strdup(filename);
    assert(res->name != NULL);

    char *mode = is_binary ? "rb" : "r";
    FILE *fp = fopen(filename, mode);
    if (!fp)
        return false;

    long saved_offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    res->size = (size_t) ftell(fp);
    fseek(fp, saved_offset, SEEK_SET);

    if (res->size == 0) {
        fclose(fp);
        return true;
    }

    res->content = malloc(res->size + 1);    
    assert(res->content != NULL);

#ifdef _WIN32
    /* \r\n */
    fread(res->content, 1, res->size, fp);
#else
    size_t read_size = fread(res->content, 1, res->size, fp);
    if (read_size != res->size) {
        fclose(fp);
        free(res->content);
        res->content = NULL;
        return false;
    }
#endif /* platform */

    res->content[res->size] = '\0';
    res->line = count_line(res->content, res->size);
    res->attr = zd_fs_get_attr(filename);

    fclose(fp);
    return true;
}

ZD_DEF void zd_fs_free_mf(struct zd_meta_file *mf)
{
    if (mf->name)
        free(mf->name);
    if (mf->content)
        free(mf->content);

    mf->name = NULL;
    mf->content = NULL;
    mf->type = FT_NOET;
    mf->size = 0;
    mf->line = 0;
}

ZD_DEF int zd_fs_get_attr(const char *filename)
{
    int perm = 0;
#ifdef _WIN32
    if (_access(filename, 4) == 0) perm |= FA_READ;
    if (_access(filename, 2) == 0) perm |= FA_WRITE;
    const char *ext = strrchr(filename, '.');
    if (ext) { 
        if (strcmp(ext, ".exe") == 0 || strcmp(ext, ".bat") == 0 ||
            strcmp(ext, ".cmd") == 0 || strcmp(ext, ".com") == 0) {
            perm |= FA_EXEC;
        }
    }
#else
    struct stat sb;
    if (stat(filename, &sb) != 0) {
        return perm;
    }

    if (sb.st_mode & S_IRUSR) perm |= FA_READ;
    if (sb.st_mode & S_IWUSR) perm |= FA_WRITE;
    if (sb.st_mode & S_IXUSR) perm |= FA_EXEC;
#endif /* platform */

    return perm;
}

ZD_DEF bool zd_fs_check_perm(const char *filename, int perm)
{
    return zd_fs_get_attr(filename) & perm;
}

ZD_DEF void zd_fs_free_md(struct zd_meta_dir *md)
{
    if (md->name)
        free(md->name);
    if (md->files) {
        for (size_t i = 0; i < md->f_cnt; i++) {
            if (md->files[i])
                free(md->files[i]);
            md->files[i] = NULL;
        }
        free(md->files);
    }
    if (md->dirs) {
        for (size_t i = 0; i < md->d_cnt; i++) {
            if (md->dirs[i])
                free(md->dirs[i]);
            md->dirs[i] = NULL;
        }
        free(md->dirs);
    }

    md->name = NULL;
    md->files = NULL;
    md->dirs = NULL;
    md->f_cnt = 0;
    md->d_cnt = 0;
    md->count = 0;
}

ZD_DEF bool zd_fs_dumpf(const char *dest_file,
        const char *src_buf, size_t size, bool is_binary)
{
    char *mode = is_binary ? "wb" : "w";
    FILE *fp = fopen(dest_file, mode);
    if (!fp) 
        return false;

    size_t write_size = fwrite(src_buf, 1, size, fp);
    if (write_size != size) {
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

/* MODULE: ZD_DS_DYNAMIC_ARRAY */

/* A iterator */
ZD_DEF void *zd_dyna_next(struct zd_dyna *da)
{
    if (da->pos > da->count)
        da->pos = 0;
    return zd_dyna_get(da, da->pos++);
}

ZD_DEF void zd_dyna_init(struct zd_dyna *da, size_t size,
        void (*clear_item)(void *))
{
    da->base = NULL;
    da->count = 0;
    da->capacity = 0;
    da->clear_item = clear_item;
    da->size = size;
    da->pos = 0;
}

ZD_DEF void zd_dyna_append(struct zd_dyna *da, void *elem)
{
    if (da->capacity <= da->count) {
        #define ZD_DA_INIT_CAP 10
        da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
        #undef ZD_DA_INIT_CAP
        da->base = realloc(da->base, da->size * da->capacity);
        assert(da->base != NULL);
    }
    void *dest = zd_dyna_get(da, da->count++);
    memcpy(dest, elem, da->size);
}

ZD_DEF void zd_dyna_insert(struct zd_dyna *da, size_t idx, void *elem)
{
    if (idx > da->count) 
        return;
    else if (idx == da->count) 
        zd_dyna_append(da, elem);
    else {
        if (da->capacity <= da->count) {
            #define ZD_DA_INIT_CAP 10
            da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
            #undef ZD_DA_INIT_CAP
            da->base = realloc(da->base, da->size * da->capacity);
            assert(da->base != NULL);
        }
        void *dest = zd_dyna_get(da, idx);
        memcpy((char *) dest + da->size, dest, (da->count-idx) * da->size);
        memcpy(dest, elem, da->size);
        da->count++;
    }
}

ZD_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx)
{
    if (da->count > 0 && idx < da->count) {
        void *elem = zd_dyna_get(da, idx);
        if (da->clear_item)
            da->clear_item(elem);
        memcpy(zd_dyna_get(da, idx), zd_dyna_get(da, idx + 1),
                da->size * (da->count - idx - 1));
        da->count--;
    }
}

/* zd_dyna_set means: { clear_item(da[idx]); da[idx] = elem; } */
ZD_DEF void zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem)
{
    if (idx >= da->count) 
        return;
    void *dest = zd_dyna_get(da, idx);
    if (da->clear_item && dest) 
        da->clear_item(dest);
    memcpy(dest, elem, da->size);
}

/* zd_dyna_get means: return da[idx]; */
ZD_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx)
{
    if (idx >= da->count) return NULL;
    return (char *) da->base + da->size * idx;
}

ZD_DEF void zd_dyna_free(struct zd_dyna *da)
{
    if (da->clear_item) {
        for (size_t i = 0; i < da->count; i++) {
            void *item = (char *) da->base + da->size * i;
            da->clear_item(item);
        }
    }
    free(da->base);
    da->count = 0;
    da->capacity = 0;
    da->clear_item = NULL;
    da->size = 0;
    da->pos = 0;
}

/* MODULE: ZD_DS_STRING */

ZD_DEF void zd_string_append(struct zd_string *str, const char *fmt, ...)
{
    if (!fmt)
        return;

    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);
    size_t len = vsnprintf(NULL, 0, fmt, args_copy);
    if (len <= 0) {
        va_end(args_copy);
        va_end(args);
        return;
    }
    va_end(args_copy);

    char new_str[len+1];
    vsnprintf(new_str, len + 1, fmt, args);
    new_str[len] = '\0';

    while (str->capacity <= str->length + len + 1) {
        #define ZD_STRING_INIT_CAP 128
        str->capacity = (str->capacity == 0) ? ZD_STRING_INIT_CAP 
                                             : (2 * str->capacity);
        #undef ZD_STRING_INIT_CAP
        str->base = realloc(str->base, str->capacity);
        assert(str->base != NULL);
    }
    memcpy(str->base + str->length, new_str, len);
    str->length += len;
    str->base[str->length] = '\0';

    va_end(args);
}

ZD_DEF struct zd_string zd_string_replace(const char *str,
        const char *s_old, const char *s_new)
{
    struct zd_string res = {0};
    char *pos = strstr(str, s_old);

    if (!pos) return res;
    size_t len_before = pos - str;

    struct zd_string tmp = {0};
    zd_string_append(&tmp, str);
    tmp.base[len_before] = '\0';

    zd_string_append(&res, tmp.base); 
    zd_string_append(&res, s_new);
    zd_string_append(&res, pos + strlen(s_old));

    zd_string_free(&tmp);
    return res; 
}

ZD_DEF struct zd_string zd_string_repeat(const char *str, size_t times)
{
    struct zd_string res = {0};

    while (times--)
        zd_string_append(&res, str);

    return res;
}

/* [src, dest) */
ZD_DEF struct zd_string zd_string_sub(const char *str, size_t src, size_t dest)
{
    struct zd_string res = {0};
    if (src >= strlen(str) || dest > strlen(str) || src >= dest) 
        return res;

    char buf[dest - src + 1];
    memcpy(buf, str + src, dest - src);
    buf[dest - src] = '\0';
    zd_string_append(&res, buf);

    return res;
}

static struct zd_string _string_concat(const char *delim, ...)
{
    va_list args;
    va_start(args, delim);

    char *p;
    struct zd_string res = {0};
    size_t pos = 0;

    while ((p = va_arg(args, char *)) != NULL) {
        pos = res.length + strlen(p);
        zd_string_append(&res, "%s%s", p, delim);
    }
    va_end(args);

    res.base[pos] = '\0';
    res.length = pos;

    return res;
}

ZD_DEF void zd_string_free(void *arg)
{
    struct zd_string *str = (struct zd_string *) arg;
    if (str->base != NULL) free(str->base);
    str->base = NULL;
    str->length = 0;
    str->capacity = 0;
}

/* MODULE: ZD_DS_STACK */

ZD_DEF void zd_stack_init(struct zd_stack *stk, size_t size,
        void (*clear_item)(void *))
{
    stk->base = NULL;
    stk->top = -1;
    stk->capacity = 0;
    stk->clear_item = clear_item;
    stk->size = size;
    if (size != 0)
        zd_dyna_init(&stk->gc, size, clear_item);
}

ZD_DEF void zd_stack_push(struct zd_stack *stk, void *elem)
{
    stk->top++;
    if (stk->capacity <= (size_t) stk->top) {
        #define ZD_STK_INIT_CAP 10
        stk->capacity = (stk->capacity == 0) ? ZD_STK_INIT_CAP : (2 * stk->capacity);
        #undef ZD_STK_INIT_CAP
        stk->base = realloc(stk->base, stk->size * stk->capacity);
        assert(stk->base != NULL);
    }
    void *dest = (char *) stk->base + stk->size * stk->top;
    memcpy(dest, elem, stk->size);
}

ZD_DEF void *zd_stack_pop(struct zd_stack *stk)
{
    if (stk->top == -1) 
        return NULL; 
    void *item = (char *) stk->base + stk->size * stk->top--;
    zd_dyna_append(&stk->gc, item);
    return item;
}

ZD_DEF void *zd_stack_top(struct zd_stack *stk)
{
    if (stk->top == -1) 
        return NULL; 
    return (char *) stk->base + stk->size * stk->top;
}

ZD_DEF void zd_stack_free(struct zd_stack *stk)
{
    if (stk->clear_item) {
        for (int i = 0; i <= stk->top; i++) {
            void *item = (char *) stk->base + stk->size * i;
            stk->clear_item(item);
        }
    }
    zd_dyna_free(&stk->gc);

    free(stk->base);
    stk->top = -1;
    stk->capacity = 0;
    stk->clear_item = NULL;
    stk->size = 0;
}

/* MODULE: ZD_DS_QUEUE */

static void *zd_queue_create_node(void *elem, size_t size)
{
    struct zd_queue_node *np = malloc(sizeof(struct zd_queue_node));
    assert(np != NULL);

    np->data = malloc(size);
    assert(np->data != NULL);
    memcpy(np->data, elem, size);
    np->next = NULL;

    return np;
}

ZD_DEF void zd_queue_init(struct zd_queue *qp, size_t size,
        void (*clear_item)(void *))
{
    qp->head = (struct zd_queue_node) {0};
    qp->front = NULL;
    qp->rear  = NULL;
    qp->count = 0;
    qp->size  = size;
    qp->clear_item = clear_item;
    if (size != 0)
        zd_dyna_init(&qp->gc, sizeof(void *), NULL);
}

ZD_DEF void zd_queue_free(struct zd_queue *qp)
{
    struct zd_queue_node *np = qp->head.next, *tmp = NULL;
    while (np) {
        tmp = np->next;
        if (qp->clear_item) 
            qp->clear_item(np->data);
        free(np->data);
        free(np);
        np = tmp;
    }

    void **gc_item;
    while ((gc_item = zd_dyna_next(&qp->gc)) != NULL) {
        if (qp->clear_item)
            qp->clear_item(*gc_item);
        free(*gc_item);
    }
    zd_dyna_free(&qp->gc);

    zd_queue_init(qp, 0, NULL);
}

ZD_DEF void zd_queue_push(struct zd_queue *qp, void *elem)
{
    struct zd_queue_node *np = zd_queue_create_node(elem, qp->size);

    if (zd_queue_isempty(qp)) {
        qp->front = np;
        qp->rear = np;
        qp->head.next = qp->front;
    } else {
        qp->rear->next = np;
        qp->rear = qp->rear->next;
    }

    qp->count++;
}

ZD_DEF void *zd_queue_pop(struct zd_queue *qp)
{
    if (zd_queue_isempty(qp)) {
        return NULL;
    } else if (qp->front == qp->rear) {
        /* just one element */
        void *res = qp->front->data;

        zd_dyna_append(&qp->gc, &qp->front->data);
        free(qp->front);
        qp->head = (struct zd_queue_node) {0};
        qp->front = NULL;
        qp->rear  = NULL;
        qp->count = 0;

        return res;
    } else {
        void *res = qp->front->data;
        qp->head.next = qp->front->next;

        zd_dyna_append(&qp->gc, &qp->front->data);
        free(qp->front);

        qp->front = qp->head.next;
        qp->count--;

        return res;
    }
}

ZD_DEF void *zd_queue_front(struct zd_queue *qp)
{
    if (zd_queue_isempty(qp))
        return NULL;
    return qp->front->data;
}

ZD_DEF void *zd_queue_rear(struct zd_queue *qp)
{
    if (zd_queue_isempty(qp))
        return NULL;
    return qp->rear->data;
}

/* MODULE: ZD_LOG */

#define ZD_LOG_COLOR_RESET      "\x1b[0m"
#define ZD_LOG_COLOR_FG_RED     "\x1b[31m"
#define ZD_LOG_COLOR_FG_GREEN   "\x1b[32m"
#define ZD_LOG_COLOR_FG_YELLOW  "\x1b[33m"
#define ZD_LOG_COLOR_BG_RED     "\x1b[41m"
#define ZD_LOG_COLOR_BG_GREEN   "\x1b[42m"

ZD_DEF void zd_log(int type, const char *fmt, ...)
{
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);
    size_t len = vsnprintf(NULL, 0, fmt, args_copy);
    if (len <= 0) {
        va_end(args_copy);
        va_end(args);
        return;
    }
    va_end(args_copy);

    size_t buf_size = len + 100;
    char buf1[buf_size], buf2[buf_size];
    bool is_exit = false;

    switch ((type)) {
    case LOG_INFO:
        snprintf(buf1, buf_size, "[%sINFO%s] %s\n", ZD_LOG_COLOR_FG_YELLOW,
                ZD_LOG_COLOR_RESET, fmt);
        break;

    case LOG_ERROR:
        snprintf(buf1, buf_size, "[%sERROR%s] %s\n", ZD_LOG_COLOR_FG_RED,
                ZD_LOG_COLOR_RESET, fmt);
        break;

    case LOG_GOOD:
        snprintf(buf1, buf_size, "[%sGOOD%s] %s\n", ZD_LOG_COLOR_FG_GREEN,
                ZD_LOG_COLOR_RESET, fmt);
        break;

    case LOG_FATAL:
        snprintf(buf1, buf_size, "[%sFATAL%s] %s\n", ZD_LOG_COLOR_BG_RED,
                ZD_LOG_COLOR_RESET, fmt);
        is_exit = true;
        break;

    case LOG_TODO:
        snprintf(buf1, buf_size, "[%sTODO%s] %s\n", ZD_LOG_COLOR_BG_GREEN,
                ZD_LOG_COLOR_RESET, fmt);
        is_exit = true;
        break;

    default:
        break;
    }

    vsnprintf(buf2, buf_size, buf1, args);
    fprintf(stderr, "%s", buf2);

    va_end(args);

    if (is_exit)
        exit(EXIT_FAILURE);
}

/* MODULE: ZD_BUILD */

static void _cmd_append_arg(struct zd_cmd *cmd, ...)
{
    va_list ap;
    va_start(ap, cmd);

    char *arg = NULL;
    while ((arg = va_arg(ap, char *)) != NULL) {
        struct zd_string str = {0};
        zd_string_append(&str, arg);
        zd_dyna_append(&cmd->args, &str);
        cmd->count++;
    }

    va_end(ap);
}

ZD_DEF void zd_cmd_print(struct zd_cmd *cmd)
{
    struct zd_string cmd_string = {0};

    struct zd_string *arg_iter = NULL;
    while ((arg_iter = zd_dyna_next(&cmd->args)) != NULL)
        zd_string_append(&cmd_string, " %s", arg_iter->base);

    zd_log(LOG_INFO, cmd_string.base);

    zd_string_free(&cmd_string);
}

ZD_DEF void zd_cmd_init(struct zd_cmd *cmd)
{
    zd_dyna_init(&cmd->args, sizeof(struct zd_string), zd_string_free);
    cmd->count = 0;
}

ZD_DEF void zd_cmd_free(struct zd_cmd *cmd)
{
    zd_dyna_free(&cmd->args);
    cmd->count = 0;
}

ZD_DEF int zd_cmd_run(struct zd_cmd *cmd)
{
    struct zd_string cmd_string = {0};

    for (size_t i = 0; i < cmd->count; i++) {
        char *arg = ((struct zd_string *) zd_dyna_get(&cmd->args, i))->base;
        zd_string_append(&cmd_string, " %s", arg);
    }
#ifdef _WIN32
    zd_string_append(&cmd_string, " %s", "-D__USE_MINGW_ANSI_STDIO");
#endif /* platform */
    zd_string_append(&cmd_string, " ");

    int status = system(cmd_string.base);

    if (status != 0) {
        zd_log(LOG_ERROR, "run failed [%s]", cmd_string.base);
        zd_string_free(&cmd_string);
        return 1;
    }
    zd_log(LOG_GOOD, "run successfully [%s]", cmd_string.base);

    zd_string_free(&cmd_string);
    return 0;
}

ZD_DEF void zd_build_init(struct zd_builder *builder)
{
    zd_dyna_init(&builder->cmds, sizeof(struct zd_cmd), NULL);
    builder->count = 0;
}

static void _build_update_self(const char *cc, int argc,
        char **argv, const char *source, ...)
{
    unsigned long long t1 = zd_fs_get_timestamp(argv[0]);
    unsigned long long t2 = zd_fs_get_timestamp(source);

    if (t1 >= t2) return;

    /* backup */
    struct zd_string src = {0};
    zd_string_append(&src, "%s.old", source);
    zd_fs_copy(source, src.base, false);
    struct zd_string exe = {0};
    zd_string_append(&exe, "%s.old", argv[0]);
    zd_fs_copy(argv[0], exe.base, true);

    struct zd_cmd cmd = {0};
    zd_cmd_init(&cmd);

    /* recompile */
    zd_cmd_append_arg(&cmd, cc);
    zd_cmd_append_arg(&cmd, "-o", argv[0], source);

    /* add extra args */
    va_list ap;
    va_start(ap, source);
    char *arg;
    while ((arg = va_arg(ap, char *)) != NULL)
        zd_cmd_append_arg(&cmd, arg);
    va_end(ap);

    /* execute */
    zd_cmd_append_arg(&cmd, "&&");
    for (int i = 0; i < argc; i++)
        zd_cmd_append_arg(&cmd, argv[i]);

    zd_log(LOG_INFO, "<< Update Self >>");

    if (zd_cmd_run(&cmd) != 0) {
        /* restore and report error */
        zd_fs_move(src.base, source, false);
        zd_fs_move(exe.base, argv[0], true);
        zd_log(LOG_FATAL, "failed to update self");
    }

    zd_fs_remove(src.base);
    zd_fs_remove(exe.base);
    zd_string_free(&src);
    zd_string_free(&exe);
    zd_cmd_free(&cmd);
    exit(0);
}

static void _build_append_cmd(struct zd_builder *builder, ...)
{
    va_list ap;
    va_start(ap, builder);

    struct zd_cmd *cmd = NULL;
    while ((cmd = va_arg(ap, struct zd_cmd *)) != NULL) {
        zd_dyna_append(&builder->cmds, cmd);
        builder->count++;
    }

    va_end(ap);
}

ZD_DEF void zd_build_print(struct zd_builder *builder)
{
    struct zd_cmd *cmd_iter = NULL;
    while ((cmd_iter = zd_dyna_next(&builder->cmds)) != NULL) {
        struct zd_string cmd_string = {0};

        struct zd_string *arg_iter = NULL;
        while ((arg_iter = zd_dyna_next(&cmd_iter->args)) != NULL)
            zd_string_append(&cmd_string, " %s", arg_iter->base);

        zd_log(LOG_INFO, cmd_string.base);

        zd_string_free(&cmd_string);
    }
}

ZD_DEF void zd_build_free(struct zd_builder *builder)
{
    struct zd_cmd *cmd_iter = NULL;
    while ((cmd_iter = zd_dyna_next(&builder->cmds)) != NULL)
        zd_cmd_free(cmd_iter);

    zd_dyna_free(&builder->cmds);

    builder->count = 0;
}

ZD_DEF int zd_build_run_sync(struct zd_builder *builder)
{
    for (size_t i = 0; i < builder->count; i++) {
        struct zd_cmd *cmd = zd_dyna_get(&builder->cmds, i);
        int status = zd_cmd_run(cmd);
        if (status != 0)
            return 1;
    }
    return 0;
}

/* MODULE: ZD_WILDCARD */

ZD_DEF bool zd_wildcard_match(const char *str, const char *pattern)
{
    if (!str || !pattern)
        return false;

    int m = strlen(str);
    int n = strlen(pattern);

    bool dp[m + 1][n + 1];
    
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++)
            dp[i][j] = false;
    }
    dp[0][0] = true;

    for (int j = 1; j <= n; j++) {
        if (pattern[j - 1] == '*') {
            dp[0][j] = dp[0][j - 1];
        }
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (pattern[j - 1] == '*')
                dp[i][j] = dp[i][j - 1] || dp[i - 1][j];
            else if (pattern[j - 1] == '?' || str[i - 1] == pattern[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
        }
    }

    return dp[m][n];
}

ZD_DEF void zd_wildcard_capture(const char *str, const char *pattern,
                                size_t *start, size_t *len)
{
    (void) str;
    (void) pattern;
    (void) start;
    (void) len;
}

#endif /* ZD_IMPLEMENTATION */
