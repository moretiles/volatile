#include <asm/instruction.h>
#include <asm/operand.h>
#include <asm/register.h>
#include <ast.h>
#include <compile.h>
#include <debug.h>
#include <ds/iestack.h>
#include <global.h>
#include <isa.h>
#include <lang/function.h>
#include <lang/type.h>
#include <sast.h>

#include <gtest/gtest.h>

#include <memory>

namespace {
TEST(global, allocator) {
    vltl_global_init();
    ASSERT_NE(nullptr, vltl_global_allocator);
}

TEST(global, table) {
    vltl_global_init();

    ASSERT_NE(nullptr, vltl_global_table_constants);
    ASSERT_NE(nullptr, vltl_global_table_globals);
    ASSERT_NE(nullptr, vltl_global_table_locals);
    ASSERT_NE(nullptr, vltl_global_table_types);
    ASSERT_NE(nullptr, vltl_global_table_functions);
    ASSERT_NE(nullptr, vltl_global_table_operations);
    ASSERT_NE(nullptr, vltl_global_table_attributes);

    Vltl_lang_operation *current_operation = NULL;

    ASSERT_FALSE(nkht_get(vltl_global_table_operations, "+", &current_operation));
    ASSERT_FALSE(nkht_get(vltl_global_table_operations, "-", &current_operation));
}

TEST(global, registers_use_and_reset) {
    vltl_global_init();
    Vltl_global_register *expect_r11, *expect_r10;
    ASSERT_EQ(0, vltl_global_registers_use(&expect_r11));
    ASSERT_EQ(0, vltl_global_registers_use(&expect_r10));
    ASSERT_EQ(0, vltl_global_registers_clear());
}
}

namespace {
int add_2(int *dest, int *src1, int *src2) {
    if(dest == NULL || src1 == NULL || src2 == NULL) {
        IESTACK_PUSHF(
            &vltl_global_errors,
            EINVAL, "provided pointers are NULL : dest = %p, src1 = %p, src2 = %p",
            (void *) dest, (void *) src1, (void *) src2
        );

        iestack_dump(&vltl_global_errors, stdout);
        return EINVAL;
    }

    if(*src1 < 0) {
        IESTACK_PUSH(&vltl_global_errors, EINVAL, "argument a is negative!");

        iestack_dump(&vltl_global_errors, stdout);
        return EINVAL;
    }

    if(*src2 < 0) {
        IESTACK_PUSH(&vltl_global_errors, EINVAL, "argument b is negative!");

        iestack_dump(&vltl_global_errors, stdout);
        return EINVAL;
    }

    *dest = *src1 + *src2;
    return 0;
}

int baz(void) {
    IESTACK_PUSH(&vltl_global_errors, ENOTRECOVERABLE, "bad from baz");

    return ENOTRECOVERABLE;
}

int bar(void) {
    int ret = baz();
    if(ret) {
        IESTACK_PUSH(&vltl_global_errors, EINVAL, "bad from bar");

        return ret;
    }

    return 0;
}

int foo(void) {
    int ret = bar();
    if(ret) {
        IESTACK_RETURN(&vltl_global_errors, EINVAL, "bad from foo");
    }

    return 0;
}

TEST(ds, ierror_simple) {
    vltl_global_init();
    int a = 2;
    int b = 3;
    int c = 0;

    ASSERT_FALSE(add_2(&c, &a, &b));

    b = -3;
    ASSERT_TRUE(add_2(&c, &a, &b));

    ASSERT_TRUE(add_2(&c, NULL, &b));

    ASSERT_TRUE(foo());
    iestack_dump(&vltl_global_errors, stdout);
}
}

namespace {
TEST(sast, operation_insert_and_compile) {
    vltl_global_init();
    Vltl_asm_operand operand_immediate_3 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64,
            .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
            .value = 3
        }
    };
    Vltl_asm_operand operand_immediate_4 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64,
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

TEST(sast, operation_connect_and_compile) {
    vltl_global_init();

    // Needs to assume it is in main
    Vltl_lang_function *created_function = (Vltl_lang_function *) varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_function));
    ASSERT_NE(nullptr, created_function);
    ASSERT_FALSE(vltl_lang_function_init(created_function, "main"));
    ASSERT_FALSE(nkht_set(vltl_global_table_functions, "main", &created_function));
    vltl_global_context.function = created_function;

    Vltl_asm_operand operand_immediate_3 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64,
            .representation = VLTL_ASM_OPERAND_IMMEDIATE_REPRESENTATION_BASE10,
            .value = 3
        }
    };
    Vltl_asm_operand operand_immediate_4 = {
        .kind = VLTL_ASM_OPERAND_KIND_IMMEDIATE,
        .as_immediate = {
            .integral_type = VLTL_LANG_TYPE_INTEGRAL_INT_SCALAR64,
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

    const size_t buf_cap = 9999;
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
    vltl_global_init();
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, nullptr, &ast_add_operation, 0));
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, ast_tree.root, &ast_eval1_operation, 0));
    ASSERT_EQ(0, vltl_ast_operation_insert(&ast_tree, ast_tree.root, &ast_eval2_operation, 1));
    ast_tree.root = nullptr;
    ast_tree.last = nullptr;
}

TEST_F(AstFixture1, operation_adopt) {
    vltl_global_init();
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
    vltl_global_init();
    // 1 * 2 + 3 * 4
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval1_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_mul_operation1));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval2_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_add_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval3_operation));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_mul_operation2));
    ASSERT_EQ(0, vltl_ast_tree_insert(&ast_tree, &ast_eval4_operation));

    char buf[9999];
    size_t buf_len = 0;
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, stdout);
    ast_tree.root = nullptr;
    ast_tree.last = nullptr;
}
}

namespace {
TEST(lexer, line_convert_simple) {
    vltl_global_init();
    {
        const char *src_line = "1 + 2 - 3 + 4 - 5";
        Vltl_lexer_line lexer_line = {};
        ASSERT_EQ(0, vltl_lexer_line_convert(&lexer_line, src_line));
        ASSERT_TRUE(vltl_lexer_line_valid(lexer_line));
    }
}
}

namespace {
class OnelineFixture1 : public testing::Test {
protected:
    void SetUp() override {
        vltl_global_init();

        // Needs to assume it is in main
        Vltl_lang_function *created_function = (Vltl_lang_function *) varena_alloc(&vltl_global_allocator, 1 * sizeof(Vltl_lang_function));
        ASSERT_NE(nullptr, created_function);
        ASSERT_FALSE(vltl_lang_function_init(created_function, "main"));
        ASSERT_FALSE(nkht_set(vltl_global_table_functions, "main", &created_function));
        vltl_global_context.function = created_function;
    }
};


TEST_F(OnelineFixture1, addsub) {
    char buf[9999];
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

TEST_F(OnelineFixture1, return_addsub) {
    char buf[9999];
    size_t buf_len = 0;
    const char *mathline = "return 3 + 4 - 2";
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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
    fflush(file);
    fclose(file);
}

TEST_F(OnelineFixture1, equals_global) {
    char buf[9999];
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

TEST_F(OnelineFixture1, define_globals) {
    char buf[9999];
    size_t buf_len = 0;
    const char *mathline = "global a = 5";
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

TEST_F(OnelineFixture1, define_constants) {
    char buf[9999];
    size_t buf_len = 0;
    const char *mathline = "constant one = 1";
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

TEST_F(OnelineFixture1, define_global) {
    char buf[9999];
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

TEST_F(OnelineFixture1, define_constant) {
    char buf[9999];
    size_t buf_len = 0;
    const char *mathline = "constant running_out_of_variable_names = 3 + 4 + 5";
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

    Vltl_lang_constant *check_if_exists = nullptr;
    ASSERT_FALSE(nkht_get(vltl_global_table_constants, "running_out_of_variable_names", &check_if_exists));
    ASSERT_NE(nullptr, check_if_exists);
}

TEST_F(OnelineFixture1, define_function) {
    char buf[9999];
    size_t buf_len = 0;
    const char *mathline = "function just_return_3 1 {\n"
                           "    return 3\n"
                           "}";
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
    vltl_ast_tree_detokenize(buf, 9999, &buf_len, ast_tree);
    fputs(buf, file);
    fflush(file);
    fclose(file);
    file = nullptr;

    // sast tree
    const char *sast_filename = "scratch/sast.dot";
    file = fopen(sast_filename, "w");
    ASSERT_EQ(0, vltl_sast_tree_convert(&sast_tree, &ast_tree));
    vltl_sast_tree_detokenize(buf, 9999, &buf_len, sast_tree);
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

    Vltl_lang_function *check_if_exists = nullptr;
    ASSERT_FALSE(nkht_get(vltl_global_table_functions, "just_return_3", &check_if_exists));
    ASSERT_NE(nullptr, check_if_exists);
}
}

namespace {
TEST(fullpass, manylines_addsub) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/manylines_addsub.bin";
    char src_filename[] = "tests/fullpass/manylines_addsub.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_globals) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/simple_globals.bin";
    char src_filename[] = "tests/fullpass/simple_globals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_locals) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/simple_locals.bin";
    char src_filename[] = "tests/fullpass/simple_locals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_constants) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/simple_constants.bin";
    char src_filename[] = "tests/fullpass/simple_constants.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, modify_globals) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/modify_globals.bin";
    char src_filename[] = "tests/fullpass/modify_globals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, define_and_use_globals) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/define_and_use_globals.bin";
    char src_filename[] = "tests/fullpass/define_and_use_globals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, return_using_globals_and_constants) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/return_using_globals_and_constants.bin";
    char src_filename[] = "tests/fullpass/return_using_globals_and_constants.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, return_using_subtraction) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/return_using_subtraction.bin";
    char src_filename[] = "tests/fullpass/return_using_subtraction.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, return_using_multiplication) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/return_using_multiplication.bin";
    char src_filename[] = "tests/fullpass/return_using_multiplication.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, return_using_division) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/return_using_division.bin";
    char src_filename[] = "tests/fullpass/return_using_division.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, main_function) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/main_function.bin";
    char src_filename[] = "tests/fullpass/main_function.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));

    Vltl_lang_function *the_main_function = nullptr;
    ASSERT_FALSE(nkht_get(vltl_global_table_functions, "main_function_main", &the_main_function));
}

TEST(fullpass, several_functions) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/several_functions.bin";
    char src_filename[] = "tests/fullpass/several_functions.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));

    Vltl_lang_function *function = nullptr;
    ASSERT_FALSE(nkht_get(vltl_global_table_functions, "several_functions_return_1", &function));
    ASSERT_FALSE(nkht_get(vltl_global_table_functions, "several_functions_return_2", &function));
    ASSERT_FALSE(nkht_get(vltl_global_table_functions, "several_functions_return_3", &function));
}

TEST(fullpass, simple_comma) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/simple_comma.bin";
    char src_filename[] = "tests/fullpass/simple_comma.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, grouping_beats_multiplication) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/grouping_beats_multiplication.bin";
    char src_filename[] = "tests/fullpass/grouping_beats_multiplication.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, function_call_one_arg) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/function_call_one_arg.bin";
    char src_filename[] = "tests/fullpass/function_call_one_arg.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, function_with_args) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/function_with_args.bin";
    char src_filename[] = "tests/fullpass/function_with_args.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, function_multilevel) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/function_multilevel.bin";
    char src_filename[] = "tests/fullpass/function_multilevel.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, use_scoping) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/use_scoping.bin";
    char src_filename[] = "tests/fullpass/use_scoping.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, return_using_test_equals) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/return_using_test_equals.bin";
    char src_filename[] = "tests/fullpass/return_using_test_equals.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, if_statement_main) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/if_statement.bin";
    char src_filename[] = "tests/fullpass/if_statement.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, elif_statement_main) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/elif_statement.bin";
    char src_filename[] = "tests/fullpass/elif_statement.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, else_statement_main) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/else_statement.bin";
    char src_filename[] = "tests/fullpass/else_statement.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, while_statement_main) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/while_statement.bin";
    char src_filename[] = "tests/fullpass/while_statement.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, address_indirection_main) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/address_indirection.bin";
    char src_filename[] = "tests/fullpass/address_indirection.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, index_into_main) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/index_into.bin";
    char src_filename[] = "tests/fullpass/index_into.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, tour_de_force) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/tour_de_force.bin";
    char src_filename[] = "tests/fullpass/tour_de_force.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}
}
