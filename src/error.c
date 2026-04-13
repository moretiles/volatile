#include <trace.h>
#include <error.h>
#include <dye.h>

_Thread_local char vltl_error_buf[VLTL_ERROR_BUF_CAP];

void vltl_error(const char *string) {
    dyebg(stderr, DYE_RED);
    fputs(string, stderr);
    dyebg(stderr, DYE_RESET);
    fputs("\n", stderr);
}

void vltl_error_line(const Vltl_lexer_line bad_line) {
    int ret = 0;
    size_t len = 0;

    iestack_dump(vltl_global_errors, stderr);

    BTRC_SNPRINTF(&ret, &len, vltl_error_buf, VLTL_ERROR_BUF_CAP, "Error at line: %zu!", bad_line.linenumber);
    vltl_error(vltl_error_buf);
    BTRC_SNPRINTF(&ret, &len, vltl_error_buf, VLTL_ERROR_BUF_CAP, "Bad line: %s", bad_line.text);
    fputs(vltl_error_buf, stderr);
    if(len > 0 && vltl_error_buf[len - 1] != '\n') {
        fputs("\n", stderr);
    }
}

void vltl_error_token(const Vltl_lang_token bad_token) {
    int ret = 0;
    size_t len = 0;
    (void) bad_token;

    if(bad_token.traced_by && bad_token.traced_by->as_line) {
        vltl_error_line(*(bad_token.traced_by->as_line));
    }

    bool done = false;
    Vltl_lexer_token *as_lexer_token = NULL;
    for(size_t i = 0; i < VLTL_LEXER_LINE_TOKENS_MAX; i++) {
        Vltl_lexer_token *const ith_lexer_token = &(bad_token.traced_by->as_line->tokens[i]);
        Vltl_lang_token *const ith_lang_token = &(bad_token.traced_by->as_line->tokens[i].token);

        if(ith_lang_token == bad_token.traced_by->as_token) {
            as_lexer_token = ith_lexer_token;
            done = true;
        }
    }
    assert(done);

    const char *line_at_offset_of_token = &(bad_token.traced_by->as_line->text[as_lexer_token->offset_into_line]);
    BTRC_SNPRINTF(
        &ret, &len, vltl_error_buf, VLTL_ERROR_BUF_CAP,
        "Error beginning at: %s", line_at_offset_of_token
    );
    fputs(vltl_error_buf, stderr);
    if(len > 0 && vltl_error_buf[len - 1] != '\n') {
        fputs("\n", stderr);
    }
}

void vltl_error_ast(const Vltl_ast_operation bad_ast) {
    size_t len = 0;

    if(bad_ast.traced_by && bad_ast.traced_by->as_token) {
        vltl_error_token(*(bad_ast.traced_by->as_token));
    }

#ifndef SOME_DEBUG_FLAG_IG
    // dump ast tree as graphviz dot to file
    if(bad_ast.belongs_to) {
        vltl_ast_tree_detokenize(vltl_error_buf, VLTL_ERROR_BUF_CAP, &len, *(bad_ast.belongs_to));
        FILE *debug_file = fopen("scratch/err_ast.dot", "w");
        assert(debug_file != NULL);
        fputs(vltl_error_buf, debug_file);
        fclose(debug_file);
        fputs("Dumped ast_tree as dot: scratch/err_ast.dot!\n", stderr);
    } else {
        fputs("Could not dump ast_tree because operation does not belong to any ast_tree!\n", stderr);
    }
#endif
}

void vltl_error_sast(const Vltl_sast_operation bad_sast) {
    size_t len = 0;

    (void) bad_sast;

    if(bad_sast.traced_by && bad_sast.traced_by->as_ast) {
        vltl_error_ast(*(bad_sast.traced_by->as_ast));
    }

#ifndef SOME_DEBUG_FLAG_IG
    // dump sast tree as graphviz dot to file
    if(bad_sast.belongs_to) {
        vltl_sast_tree_detokenize(vltl_error_buf, VLTL_ERROR_BUF_CAP, &len, *(bad_sast.belongs_to));
        FILE *debug_file = fopen("scratch/err_sast.dot", "w");
        assert(debug_file != NULL);
        fputs(vltl_error_buf, debug_file);
        fclose(debug_file);
        fputs("Dumped sast_tree as dot: scratch/err_sast.dot!\n", stderr);
    } else {
        fputs("Could not dump sast_tree because operation does not belong to any sast_tree!\n", stderr);
    }
#endif
}

void vltl_error_asm(const char *bad_asm) {
    (void) bad_asm;

    //vltl_error_sast(bad_ast->trace->sast);
#ifdef SOME_DEBUG_FLAG_IG
    // print assembly listing
#endif
}
