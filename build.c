#define ZD_IMPLEMENTATION
#include "zd2.h"

#define CC          "gcc"
#define TARGET      "velo"
#define FRONT_END   "src/frontend/"
#define BACK_END    "src/backend/"

static void compile(void)
{
    builder_t builder = {0};
    build_init(&builder, TARGET, "src/", true);

    /* compile */
    for (size_t i = 0; i < builder.srcs.count; i++) {
        string_t *src = (string_t *) dyna_get(&builder.srcs, i);
        string_t *obj = (string_t *) dyna_get(&builder.objs, i);
        if (fs_get_timestamp(src->base) <= fs_get_timestamp(obj->base)) continue;
        cmd_t cmd = {0};
        cmd_init(&cmd);
        cmd_append_arg(&cmd, CC, "-I", "inc/", "-Wall", "-Wextra",
                "-c", "-o", obj->base, src->base);
        build_append_cmd(&builder, &cmd);
    }

    /* link */
    cmd_t cmd = {0};
    cmd_init(&cmd);
    cmd_append_arg(&cmd, CC, "-I", "inc/", "-Wall", "-Wextra",
            "-o", builder.target.base);
    for (size_t i = 0; i < builder.objs.count; i++) {
        string_t *obj = (string_t *) dyna_get(&builder.objs, i);
        cmd_append_arg(&cmd, obj->base);
    }
    build_append_cmd(&builder, &cmd);

    build_run_sync(&builder);

    build_free(&builder);
}

static void clean(void)
{
#ifdef _WIN32
    dyna_t files = fs_match_recursively("src/", "*.obj");
#else
    dyna_t files = fs_match_recursively("src/", "*.o");
#endif
    fs_remove_all(&files);
    fs_remove(TARGET);
    dyna_free(&files);
}

static void define_rule(cmdl_t *cmdl)
{
    cmdl_define(cmdl, OPTT_NO_ARG, "help", "h", "Print this information");
    cmdl_define(cmdl, OPTT_NO_ARG, "compile", "c", "Compile all source files");
    cmdl_define(cmdl, OPTT_NO_ARG, "clean", "cl", "Clean all generated files");
}

int main(int argc, char **argv)
{
    build_update_self(CC, argc, argv);

    cmdl_t cmdl = {0};
    cmdl_init(&cmdl, true);
    define_rule(&cmdl);

    cmdl_build(&cmdl, argc, argv);

    bool is_help = cmdl_isuse(&cmdl, "help");
    bool is_compile = cmdl_isuse(&cmdl, "compile");
    bool is_clean = cmdl_isuse(&cmdl, "clean");

    if (is_help) cmdl_usage(&cmdl);
    if (is_compile) compile();
    if (is_clean) clean();

    cmdl_free(&cmdl);

    return 0;
}

