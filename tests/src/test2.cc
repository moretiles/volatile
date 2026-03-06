#include <gtest/gtest.h>

namespace {
extern "C" {
    long manylines_addsub_main(void);
    long define_and_use_globals_main(void);
    long main_function_main(void);
    long modify_globals_main(void);
    long return_using_globals_and_constants_main(void);
    long simple_locals_main(void);
    long simple_globals_main(void);
    long simple_constants_main(void);
    long several_functions_return_1(void);
    long several_functions_return_2(void);
    long several_functions_return_3(void);
    long return_using_subtraction_main(void);
    long return_using_multiplication_main(void);
    long return_using_division_main(void);
    long grouping_beats_multiplication_main(void);
    long simple_comma_main(void);
    long function_call_one_arg_main(void);
    long function_with_args_main(void);
    long function_with_args_zero(void);
    long function_with_args_one(long a);
    long function_with_args_two(long a, long b);
    long function_with_args_three(long a, long b, long c);
    long function_with_args_four(long a, long b, long c, long d);
    long function_multilevel_main(void);
    long use_scoping_main(void);
    long return_using_test_equals_main(void);
    long if_statement_main(void);
    long elif_statement_main(void);
    long else_statement_main(void);
    long while_statement_main(void);
    long tour_de_force_main(void);
}

TEST(file, manylines_addsub) {
    ASSERT_EQ(5, manylines_addsub_main());
}

TEST(file, define_and_use_globals) {
    ASSERT_EQ(375, define_and_use_globals_main());
}

TEST(file, main_function) {
    ASSERT_EQ(15, main_function_main());
}

TEST(file, modify_globals) {
    ASSERT_EQ(28, modify_globals_main());
}

TEST(file, return_using_globals_and_constants) {
    ASSERT_EQ(12, return_using_globals_and_constants_main());
}

TEST(file, simple_locals) {
    ASSERT_EQ(4, simple_locals_main());
}

TEST(file, simple_globals) {
    ASSERT_EQ(6, simple_globals_main());
}

TEST(file, simple_constans) {
    ASSERT_EQ(1, simple_constants_main());
}

TEST(file, several_functions) {
    ASSERT_EQ(1, several_functions_return_1());
    ASSERT_EQ(2, several_functions_return_2());
    ASSERT_EQ(3, several_functions_return_3());
}

TEST(file, return_using_subtraction) {
    ASSERT_EQ(10, return_using_subtraction_main());
}

TEST(file, return_using_multiplication) {
    ASSERT_EQ(6, return_using_multiplication_main());
}

TEST(file, return_using_division) {
    ASSERT_EQ(500, return_using_division_main());
}

TEST(file, grouping_beats_multiplication) {
    ASSERT_EQ(62, grouping_beats_multiplication_main());
}

TEST(file, simple_comma) {
    ASSERT_EQ(3, simple_comma_main());
}

TEST(file, function_call_one_arg) {
    ASSERT_EQ(9, function_call_one_arg_main());
}

TEST(file, function_with_args) {
    ASSERT_EQ(3611, function_with_args_main());
    ASSERT_EQ(100, function_with_args_zero());
    ASSERT_EQ(5, function_with_args_one(5));
    ASSERT_EQ(73, function_with_args_two(7, 3));
    ASSERT_EQ(241, function_with_args_three(2, 4, 1));
    ASSERT_EQ(3192, function_with_args_four(3, 1, 9, 2));

    ASSERT_EQ(
        3611,
        function_with_args_four(3, 1, 9, 2) +
        function_with_args_three(2, 4, 1) +
        function_with_args_two(7, 3) +
        function_with_args_one(5) +
        function_with_args_zero()
    );
}

TEST(file, function_multilevel) {
    ASSERT_EQ(32, function_multilevel_main());
}

TEST(file, use_scoping) {
    ASSERT_EQ(11, use_scoping_main());
}

TEST(file, return_using_test_equals) {
    ASSERT_EQ(1, return_using_test_equals_main());
}

TEST(file, if_statement) {
    ASSERT_EQ(7832, if_statement_main());
}

TEST(file, elif_statement) {
    ASSERT_EQ(3000, elif_statement_main());
}

TEST(file, else_statement) {
    ASSERT_EQ(35, else_statement_main());
}

TEST(file, while_statement) {
    ASSERT_EQ(990, while_statement_main());
}

TEST(file, tour_de_force) {
    ASSERT_EQ(95, tour_de_force_main());
}
}
