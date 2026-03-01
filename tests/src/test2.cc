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
    ASSERT_EQ(15, grouping_beats_multiplication_main());
}

TEST(file, simple_comma) {
    ASSERT_EQ(3, simple_comma_main());
}

TEST(file, function_call_one_arg) {
    ASSERT_EQ(3, function_call_one_arg_main());
}
}
