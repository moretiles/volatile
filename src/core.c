#include <asm/core.c>
#include <ast.c>
#include <convert.c>
#include <compile.c>
#include <debug.c>
#include <ds/core.c>
#include <isa.c>
#include <global.c>
#include <lang/core.c>
#include <lexer.c>
#include <sast.c>
#include <trace.c>
#include <error.c>
#include <reshape.c>
#include <siphash.c>
#include <dye.c>

#include <ds/iestack.h>

int main(int argc, char *argv[]) {
    const char *dot_bin = ".bin";
    const size_t length_of_dot_bin = strlen(dot_bin);

    if(argc < 2) {
        fprintf(stderr, "./vltl {source.vltl}\n");
        return EXIT_FAILURE;
    }
    char *source_filename = argv[1];

    char *last_dot = NULL, *current_dot = source_filename;
    while((current_dot = strchr(current_dot, '.'))) {
        last_dot = current_dot++;
    }
    size_t length_before_dot = strlen(source_filename);
    if(last_dot != NULL) {
        length_before_dot = last_dot - source_filename;
    }

    char *destination_filename = calloc(1, length_before_dot + length_of_dot_bin + 1);
    assert(destination_filename != NULL);
    memcpy(destination_filename, source_filename, length_before_dot);
    memcpy(&(destination_filename[length_before_dot]), dot_bin, length_of_dot_bin);
    destination_filename[length_before_dot + length_of_dot_bin] = 0;

    int ret = vltl_compile_file(destination_filename, source_filename);

    if(destination_filename) {
        free(destination_filename);
        destination_filename = NULL;
    }

    if(ret) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
