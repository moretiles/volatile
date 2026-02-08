#include <debug.h>
#include <ast.h>
#include <asm/instruction.h>
#include <asm/operand.h>
#include <asm/register.h>
#include <compile.h>
#include <global.h>
#include <isa.h>
#include <lang/type.h>
#include <sast.h>

#include <gtest/gtest.h>

#include <memory>

namespace {
TEST(vltl_sast, operation_insert_and_compile) {
    Vltl_asm_operand operand_immediate_3 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
            .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
            .value = 3
        }
    };
    Vltl_asm_operand operand_immediate_4 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
            .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
            .value = 4
        }
    };
    Vltl_sast_operation load_suboperation_A = {
        .kind = VLTL_SAST_OPERATION_KIND_LOAD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = operand_immediate_3,
        .destination = vltl_asm_operand_amd64_rcx
    };
    Vltl_sast_operation load_suboperation_B = {
        .kind = VLTL_SAST_OPERATION_KIND_LOAD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = operand_immediate_4,
        .destination = vltl_asm_operand_amd64_rdx
    };
    Vltl_sast_operation add_suboperation = {
        .kind = VLTL_SAST_OPERATION_KIND_ADD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = {},
        .destination = vltl_asm_operand_amd64_rdx
    };
    Vltl_sast_operation store_suboperation = {
        .kind = VLTL_SAST_OPERATION_KIND_STORE,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = {},
        .destination = vltl_asm_operand_amd64_rcx
    };

    Vltl_sast_tree tree = {};
    Vltl_sast_operation *created_ptr = NULL;

    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, NULL, &store_suboperation, 0));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root, &add_suboperation, 0));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[0], &load_suboperation_A, 0));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[0], &load_suboperation_B, 1));
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[0]->arguments[0], operand_immediate_3, 0
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[0]->arguments[0], vltl_asm_operand_amd64_rcx, 1
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[0]->arguments[1], operand_immediate_4, 0
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[0]->arguments[1], vltl_asm_operand_amd64_rdx, 1
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root, vltl_asm_operand_amd64_rcx, 1
        )
    );

    fputs(".global main\n", stdout);
    fputs("\n", stdout);
    fputs(".text\n", stdout);
    fputs("main:\n", stdout);
    ASSERT_EQ(0, vltl_compile_convert(stdout, &tree));
}

TEST(vltl_sast, operation_connect_and_compile) {
    Vltl_asm_operand operand_immediate_3 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
            .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
            .value = 3
        }
    };
    Vltl_asm_operand operand_immediate_4 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT64,
            .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
            .value = 4
        }
    };
    Vltl_sast_operation load_suboperation_A = {
        .kind = VLTL_SAST_OPERATION_KIND_LOAD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = operand_immediate_3,
        .destination = {}
    };
    Vltl_sast_operation load_suboperation_B = {
        .kind = VLTL_SAST_OPERATION_KIND_LOAD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = operand_immediate_4,
        .destination = {}
    };
    Vltl_sast_operation load_suboperation_C = {
        .kind = VLTL_SAST_OPERATION_KIND_LOAD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = operand_immediate_3,
        .destination = {}
    };
    Vltl_sast_operation load_suboperation_D = {
        .kind = VLTL_SAST_OPERATION_KIND_LOAD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = operand_immediate_4,
        .destination = {}
    };
    Vltl_sast_operation add_suboperation1 = {
        .kind = VLTL_SAST_OPERATION_KIND_ADD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = {},
        .destination = {}
    };
    Vltl_sast_operation add_suboperation2 = {
        .kind = VLTL_SAST_OPERATION_KIND_ADD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = {},
        .destination = {}
    };
    Vltl_sast_operation add_suboperation3 = {
        .kind = VLTL_SAST_OPERATION_KIND_ADD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = {},
        .destination = {}
    };
    Vltl_sast_operation store_suboperation = {
        .kind = VLTL_SAST_OPERATION_KIND_STORE,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = {},
        .evaluates_to = {},
        .destination = vltl_asm_operand_amd64_rcx
    };

    Vltl_sast_tree tree = {};
    Vltl_sast_operation *created_ptr = NULL;

    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, NULL, &store_suboperation, 0));
    ASSERT_FALSE(
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root, vltl_asm_operand_tbd, 0
        )
    );
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root, &add_suboperation1, 1));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[1], &add_suboperation2, 0));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[1], &add_suboperation3, 1));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[1]->arguments[0], &load_suboperation_A, 0));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[1]->arguments[0], &load_suboperation_C, 1));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[1]->arguments[1], &load_suboperation_B, 0));
    ASSERT_EQ(0, vltl_sast_operation_insert(&tree, tree.root->arguments[1]->arguments[1], &load_suboperation_D, 1));
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[0]->arguments[0], vltl_asm_operand_tbd, 0
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[0]->arguments[1], vltl_asm_operand_tbd, 0
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[1]->arguments[0], vltl_asm_operand_tbd, 0
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[1]->arguments[1], vltl_asm_operand_tbd, 0
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[0]->arguments[0], operand_immediate_3, 1
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[0]->arguments[1], operand_immediate_3, 1
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[1]->arguments[0], operand_immediate_4, 1
        )
    );
    ASSERT_EQ(
        0,
        vltl_sast_operation_insert_operand(
            &tree, &created_ptr, tree.root->arguments[1]->arguments[1]->arguments[1], operand_immediate_4, 1
        )
    );
    ASSERT_EQ(0, vltl_sast_tree_connect(&tree));

    fputs(".global main\n", stdout);
    fputs("\n", stdout);
    fputs(".text\n", stdout);
    fputs("main:\n", stdout);
    ASSERT_EQ(0, vltl_compile_convert(stdout, &tree));

    fputs("\n", stdout);
    fputs("\n", stdout);

    const size_t buf_cap = 999;
    size_t buf_len = 0;
    char buf[buf_cap];
    ASSERT_EQ(
        0,
        vltl_sast_tree_detokenize(
            buf, buf_cap, &buf_len, tree
        )
    );
    fputs(buf, stdout);
}
}

namespace {
class AstFixture1 : public testing::Test {
public:
    Vltl_ast_tree ast_tree = { 0 };
    Vltl_ast_operation *created_child = nullptr;
    Vltl_lang_token ast_eval1_operation_evaluates = {
        .kind = VLTL_LANG_TOKEN_KIND_LITERAL,
        .literal = {
            .type = &vltl_lang_type_long,
            .fields = { (void *) 0x1 }
        }
    };
    Vltl_ast_operation ast_eval1_operation = {
        .kind = VLTL_AST_OPERATION_KIND_EVAL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_eval1_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_lang_token ast_eval2_operation_evaluates = {
        .kind= VLTL_LANG_TOKEN_KIND_LITERAL,
        .literal = {
            .type = &vltl_lang_type_long,
            .fields = { (void *) 0x2 }
        }
    };
    Vltl_ast_operation ast_eval2_operation = {
        .kind = VLTL_AST_OPERATION_KIND_EVAL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_eval2_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_lang_token ast_eval3_operation_evaluates = {
        .kind= VLTL_LANG_TOKEN_KIND_LITERAL,
        .literal = {
            .type = &vltl_lang_type_long,
            .fields = { (void *) 0x3 }
        }
    };
    Vltl_ast_operation ast_eval3_operation = {
        .kind = VLTL_AST_OPERATION_KIND_EVAL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_eval3_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_lang_token ast_eval4_operation_evaluates = {
        .kind= VLTL_LANG_TOKEN_KIND_LITERAL,
        .literal = {
            .type = &vltl_lang_type_long,
            .fields = { (void *) 0x4 }
        }
    };
    Vltl_ast_operation ast_eval4_operation = {
        .kind = VLTL_AST_OPERATION_KIND_EVAL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_eval4_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_lang_token ast_eval5_operation_evaluates = {
        .kind= VLTL_LANG_TOKEN_KIND_LITERAL,
        .literal = {
            .type = &vltl_lang_type_long,
            .fields = { (void *) 0x5 }
        }
    };
    Vltl_ast_operation ast_eval5_operation = {
        .kind = VLTL_AST_OPERATION_KIND_EVAL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_eval5_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_lang_token ast_add_operation_evaluates = {};
    Vltl_ast_operation ast_add_operation = {
        .kind = VLTL_AST_OPERATION_KIND_ADD,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_add_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_lang_token ast_mul_operation_evaluates = {};
    Vltl_ast_operation ast_mul_operation1 = {
        .kind = VLTL_AST_OPERATION_KIND_MUL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_mul_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_ast_operation ast_mul_operation2 = {
        .kind = VLTL_AST_OPERATION_KIND_MUL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_mul_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
    Vltl_ast_operation ast_mul_operation3 = {
        .kind = VLTL_AST_OPERATION_KIND_MUL,
        .belongs_to = nullptr,
        .parent = nullptr,
        .arguments = { nullptr },
        .equivalent = nullptr,
        .evaluates_to = &ast_mul_operation_evaluates,
        .result_type = &vltl_lang_type_long
    };
};

TEST_F(AstFixture1, operation_insert) {
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, nullptr, &ast_add_operation, 0));
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, ast_tree.root, &ast_eval1_operation, 0));
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, ast_tree.root, &ast_eval2_operation, 1));
    ast_tree.root = nullptr;
    ast_tree.last = nullptr;
}

TEST_F(AstFixture1, operation_adopt) {
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, nullptr, &ast_add_operation, 0));
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, ast_tree.root, &ast_eval1_operation, 0));
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, ast_tree.root, &ast_eval2_operation, 1));
    ASSERT_EQ(
        0,
        vltl_ast_operation_adopt(&ast_tree, &ast_mul_operation1, ast_tree.root->arguments[0])
    );
    ASSERT_EQ(
        0,
        vltl_ast_operation_insert(&ast_tree, ast_tree.root->arguments[0], &ast_eval3_operation, 1)
    );
    ast_tree.root = nullptr;
    ast_tree.last = nullptr;
}

TEST_F(AstFixture1, tree_insert) {
    // 1 * 2 + 3 * 4
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval1_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_mul_operation1));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval2_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_add_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval3_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_mul_operation2));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval4_operation));

    char buf[999];
    size_t buf_len = 0;
    vltl_ast_tree_detokenize(buf, 999, &buf_len, ast_tree);
    fputs(buf, stdout);
    ast_tree.root = nullptr;
    ast_tree.last = nullptr;
}
}

namespace {
TEST(vltl_global, table) {
    Vltl_lang_operation *current_operation = NULL;
    Vltl_lang_global *current_global = NULL;
    Vltl_lang_global *current_local = NULL;

    ASSERT_FALSE(nkht_get(vltl_global_table_operations, "+", &current_operation));
    ASSERT_FALSE(nkht_get(vltl_global_table_operations, "-", &current_operation));

    ASSERT_FALSE(nkht_get(vltl_global_table_globals, "a", &current_global));
    ASSERT_FALSE(nkht_get(vltl_global_table_globals, "b", &current_global));

    ASSERT_FALSE(nkht_get(vltl_global_table_locals, "c", &current_local));
    ASSERT_FALSE(nkht_get(vltl_global_table_locals, "d", &current_local));
}

TEST(vltl_global, registers_use_and_reset) {
    Vltl_global_register *expect_r11, *expect_r10;
    ASSERT_EQ(0, vltl_global_registers_use(&expect_r11));
    ASSERT_EQ(0, vltl_global_registers_use(&expect_r10));
    ASSERT_EQ(0, vltl_global_registers_clear());
}
}

namespace {
TEST(vltl_lexer, line_convert_simple) {
    {
        const char *src_line = "1 + 2 - 3 + 4 - 5";
        Vltl_lexer_line lexer_line = {};
        ASSERT_EQ(0, vltl_lexer_line_convert(&lexer_line, src_line));
        ASSERT_TRUE(vltl_lexer_line_valid(lexer_line));
    }
}
}

namespace {
TEST(fullpass, oneline_addsub) {
    char buf[999];
    size_t buf_len = 0;
    const char *mathline = "3 + 4 - 2";
    Vltl_lexer_line line = { 0 };
    Vltl_ast_tree ast_tree = { 0 };
    Vltl_sast_tree sast_tree = { 0 };
    FILE *file = nullptr;

    // lexer
    ASSERT_EQ(0, vltl_lexer_line_convert(&line, mathline));

    // ast tree
    const char *ast_filename = "scratch/ast.dot";
    file = fopen(ast_filename, "w");
    ASSERT_EQ(0, vltl_ast_tree_convert(&ast_tree, &line));
    vltl_ast_tree_detokenize(buf, 999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 999, &buf_len, sast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // compile
    const char *asm_filename = "scratch/asm.S";
    file = fopen(asm_filename, "w");
    ASSERT_NE(nullptr, file);
    fputs(".intel_syntax\n", file);
    fputs("\n", file);
    fputs(".global main\n", file);
    fputs("\n", file);
    fputs("main:\n", file);
    ASSERT_EQ(0, vltl_compile_convert(file, &sast_tree));
    fputs("mov %rax, %r11\n", file);
    fputs("ret\n", file);
    fflush(file);
    fclose(file);
}

TEST(fullpass, oneline_equals_global) {
    char buf[999];
    size_t buf_len = 0;
    const char *mathline = "a = 6 + 7";
    Vltl_lexer_line line = { 0 };
    Vltl_ast_tree ast_tree = { 0 };
    Vltl_sast_tree sast_tree = { 0 };
    FILE *file = nullptr;

    // lexer
    ASSERT_EQ(0, vltl_lexer_line_convert(&line, mathline));

    // ast tree
    const char *ast_filename = "scratch/ast.dot";
    file = fopen(ast_filename, "w");
    ASSERT_EQ(0, vltl_ast_tree_convert(&ast_tree, &line));
    vltl_ast_tree_detokenize(buf, 999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 999, &buf_len, sast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // compile
    const char *asm_filename = "scratch/asm.S";
    file = fopen(asm_filename, "w");
    ASSERT_NE(nullptr, file);
    fputs(".intel_syntax\n", file);
    fputs("\n", file);
    fputs(".global main\n", file);
    fputs("\n", file);
    fputs("main:\n", file);
    ASSERT_EQ(0, vltl_compile_convert(file, &sast_tree));
    fputs("mov %rax, %r11\n", file);
    fputs("ret\n", file);
    fflush(file);
    fclose(file);
}

TEST(fullpass, oneline_define_global) {
    char buf[999];
    size_t buf_len = 0;
    const char *mathline = "global abc = 1 + 2 + 3";
    Vltl_lexer_line line = { 0 };
    Vltl_ast_tree ast_tree = { 0 };
    Vltl_sast_tree sast_tree = { 0 };
    FILE *file = nullptr;

    // lexer
    ASSERT_EQ(0, vltl_lexer_line_convert(&line, mathline));

    // ast tree
    const char *ast_filename = "scratch/ast.dot";
    file = fopen(ast_filename, "w");
    ASSERT_EQ(0, vltl_ast_tree_convert(&ast_tree, &line));
    vltl_ast_tree_detokenize(buf, 999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 999, &buf_len, sast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // compile
    const char *asm_filename = "scratch/asm.S";
    file = fopen(asm_filename, "w");
    ASSERT_NE(nullptr, file);
    fputs(".intel_syntax\n", file);
    fputs("\n", file);
    ASSERT_EQ(0, vltl_compile_convert(file, &sast_tree));
    fflush(file);
    fclose(file);

    Vltl_lang_global *check_if_exists = nullptr;
    ASSERT_FALSE(nkht_get(vltl_global_table_globals, "abc", &check_if_exists));
    ASSERT_NE(nullptr, check_if_exists);
}

TEST(fullpass, manylines_addsub) {
    char dest_filename[] = "tests/fullpass/manylines_addsub.bin";
    char src_filename[] = "tests/fullpass/manylines_addsub.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_globals) {
    char dest_filename[] = "tests/fullpass/simple_globals.bin";
    char src_filename[] = "tests/fullpass/simple_globals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_locals) {
    char dest_filename[] = "tests/fullpass/simple_locals.bin";
    char src_filename[] = "tests/fullpass/simple_locals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_constants) {
    char dest_filename[] = "tests/fullpass/simple_constants.bin";
    char src_filename[] = "tests/fullpass/simple_constants.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, modify_globals) {
    char dest_filename[] = "tests/fullpass/modify_globals.bin";
    char src_filename[] = "tests/fullpass/modify_globals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, define_and_use_globals) {
    char dest_filename[] = "tests/fullpass/define_and_use_globals.bin";
    char src_filename[] = "tests/fullpass/define_and_use_globals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}
}
