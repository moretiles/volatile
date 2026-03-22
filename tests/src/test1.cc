#include <asm/instruction.h>
#include <asm/operand.h>
#include <asm/register.h>
#include <ast.h>
#include <compile.h>
#include <debug.h>
#include <ds/iestack.h>
#include <global.h>
#include <isa.h>
#include <error.h>
#include <trace.h>
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
        IESTACK_PUSHF2(
            vltl_global_errors,
            EINVAL, "provided pointers are NULL : dest = %p, src1 = %p, src2 = %p",
            (void *) dest, (void *) src1, (void *) src2
        );

        iestack_dump(vltl_global_errors, stdout);
        return EINVAL;
    }

    if(*src1 < 0) {
        IESTACK_PUSH2(vltl_global_errors, EINVAL, "argument a is negative!");

        iestack_dump(vltl_global_errors, stdout);
        return EINVAL;
    }

    if(*src2 < 0) {
        IESTACK_PUSH2(vltl_global_errors, EINVAL, "argument b is negative!");

        iestack_dump(vltl_global_errors, stdout);
        return EINVAL;
    }

    *dest = *src1 + *src2;
    return 0;
}

int baz(void) {
    IESTACK_SUPPOSE_CALLBACK(
        false,
        ENOTRECOVERABLE,
        vltl_error("callback for bad from baz!"),
        "bad from baz"
    );
}

int bar(void) {
    IESTACK_HANDLE_CALLBACK(baz(), puts("callback for bad from bar!"), "bad from bar!");

    return 0;
}

int foo(void) {
    // will travel to foo_error
    IESTACK_HANDLE_GOTO(bar(), foo_error, "bad from foo!");
    return 0;

foo_error:
    return iestack_last_error;
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
    iestack_dump(vltl_global_errors, stdout);
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

TEST(fullpass, external_function) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/external_function.bin";
    char src_filename[] = "tests/fullpass/external_function.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, write_chars) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/write_chars.bin";
    char src_filename[] = "tests/fullpass/write_chars.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, simple_attributes) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/simple_attributes.bin";
    char src_filename[] = "tests/fullpass/simple_attributes.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}

TEST(fullpass, tour_de_force) {
    vltl_global_init();
    char dest_filename[] = "tests/fullpass/tour_de_force.bin";
    char src_filename[] = "tests/fullpass/tour_de_force.vltl";
    ASSERT_FALSE(vltl_compile_file(dest_filename, src_filename));
}
}
