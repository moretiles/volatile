CFLAGS=-Wall -Wextra -Wpedantic --std=gnu11 -fwrapv -fmax-errors=5 -Wno-unused-command-line-argument -Wno-unused-label -lm
CXXFLAGS=-Wall -Wextra -Wpedantic --std=gnu++20 -fwrapv -Wno-missing-field-initializers -Wno-nested-anon-types -Wno-gnu-anonymous-struct -Wno-unused-command-line-argument
DEBUG=-g3 -ggdb -DVLTL_DEBUG=1 -DNKHT_SIPHASH_RANDOMIZE_DISABLE=1
OPTIMIZE=-O3

STATIC=-static -static-libgcc -static-libstdc++
INCLUDE=-I ./header/ -I ./src/
INCLUDE_TEST=
LIBRARIES=
LIBRARIES_LINUX=
LIBRARIES_WINDOWS=-lbcrypt
LIBRARIES_TEST=-lgtest -lgtest_main

ANALYZE_GCC=-fanalyzer
ANALYZE_CLANG=-analyze-headers

objs = 
objs += obj/asm.o
objs += obj/ast.o
objs += obj/compile.o
objs += obj/debug.o
objs += obj/isa.o
objs += obj/lang.o
objs += obj/global.o
objs += obj/lexer.o
objs += obj/sast.o
objs += obj/convert.o
objs += obj/trace.o
objs += obj/error.o
objs += obj/reshape.o
objs += obj/ds.o
objs += obj/siphash.o
objs += obj/dye.o

test_asm_files = 
test_asm_files += tests/fullpass/manylines_addsub.bin.S
test_asm_files += tests/fullpass/define_and_use_globals.bin.S
test_asm_files += tests/fullpass/main_function.bin.S
test_asm_files += tests/fullpass/modify_globals.bin.S
test_asm_files += tests/fullpass/return_using_globals_and_constants.bin.S
test_asm_files += tests/fullpass/simple_locals.bin.S
test_asm_files += tests/fullpass/simple_globals.bin.S
test_asm_files += tests/fullpass/simple_constants.bin.S
test_asm_files += tests/fullpass/several_functions.bin.S
test_asm_files += tests/fullpass/return_using_subtraction.bin.S
test_asm_files += tests/fullpass/return_using_multiplication.bin.S
test_asm_files += tests/fullpass/return_using_division.bin.S
test_asm_files += tests/fullpass/grouping_beats_multiplication.bin.S
test_asm_files += tests/fullpass/simple_comma.bin.S
test_asm_files += tests/fullpass/function_call_one_arg.bin.S
test_asm_files += tests/fullpass/function_with_args.bin.S
test_asm_files += tests/fullpass/function_multilevel.bin.S
test_asm_files += tests/fullpass/use_scoping.bin.S
test_asm_files += tests/fullpass/return_using_test_equals.bin.S
test_asm_files += tests/fullpass/if_statement.bin.S
test_asm_files += tests/fullpass/elif_statement.bin.S
test_asm_files += tests/fullpass/else_statement.bin.S
test_asm_files += tests/fullpass/while_statement.bin.S
test_asm_files += tests/fullpass/address_indirection.bin.S
test_asm_files += tests/fullpass/index_into.bin.S
test_asm_files += tests/fullpass/external_function.bin.S
test_asm_files += tests/fullpass/write_chars.bin.S
test_asm_files += tests/fullpass/simple_attributes.bin.S
test_asm_files += tests/fullpass/tour_de_force.bin.S

#######################################################################
#                                                                     #
# If you want to actually build then run `make all`                   #
#                                                                     #
#######################################################################
all: vltl

clean:
	rm -f test tags *.ast *.pch *.plist *.o externalDefMap.txt gmon.out obj/*.o




## recipes
vltl: src/core.c libvltl.a
	${CC} ${STATIC} ${OPTIMIZE} ${CFLAGS} src/core.c -o vltl ${INCLUDE} ${LIBRARIES}

libvltl.a: ${objs}
	ar rcs libvltl.a obj/*.o

vltl_amd64_linux: src/core.c libvltl.a
	clang ${STATIC} --target=x86_64-linux-gnu ${OPTIMIZE} ${CFLAGS} src/core.c -o vltl ${INCLUDE} ${LIBRARIES}

vltl_amd64_windows.exe: src/core.c libvltl.a
	clang ${STATIC} --target=x86_64-windows-gnu -DEXFULL=54 ${OPTIMIZE} ${CFLAGS} src/core.c -o vltl ${INCLUDE} ${LIBRARIES} ${LIBRARIES_WINDOWS}

obj/asm.o: src/asm/*.c header/asm/*.h
	${CC} ${DEBUG} ${CFLAGS} src/asm/core.c -c -o obj/asm.o ${INCLUDE} ${LIBRARIES}

obj/ast.o: src/ast.c header/ast.h
	${CC} ${DEBUG} ${CFLAGS} src/ast.c -c -o obj/ast.o ${INCLUDE} ${LIBRARIES}

obj/debug.o: src/debug.c header/debug.h
	${CC} ${DEBUG} ${CFLAGS} src/debug.c -c -o obj/debug.o ${INCLUDE} ${LIBRARIES}

obj/compile.o: src/compile.c header/compile.h
	${CC} ${DEBUG} ${CFLAGS} src/compile.c -c -o obj/compile.o ${INCLUDE} ${LIBRARIES}

obj/isa.o: src/isa.c header/isa.h
	${CC} ${DEBUG} ${CFLAGS} src/isa.c -c -o obj/isa.o ${INCLUDE} ${LIBRARIES}

obj/global.o: src/global.c header/global.h
	${CC} ${DEBUG} ${CFLAGS} src/global.c -c -o obj/global.o ${INCLUDE} ${LIBRARIES}

obj/lang.o: src/lang/*.c header/lang/*.h
	${CC} ${DEBUG} ${CFLAGS} src/lang/core.c -c -o obj/lang.o ${INCLUDE} ${LIBRARIES}

obj/lexer.o: src/lexer.c header/lexer.h
	${CC} ${DEBUG} ${CFLAGS} src/lexer.c -c -o obj/lexer.o ${INCLUDE} ${LIBRARIES}

obj/sast.o: src/sast.c header/sast.h
	${CC} ${DEBUG} ${CFLAGS} src/sast.c -c -o obj/sast.o ${INCLUDE} ${LIBRARIES}

obj/convert.o: src/convert.c header/convert.h
	${CC} ${DEBUG} ${CFLAGS} src/convert.c -c -o obj/convert.o ${INCLUDE} ${LIBRARIES}

obj/ds.o: src/ds/*.c header/ds/*.h
	${CC} ${DEBUG} ${CFLAGS} src/ds/core.c -c -o obj/ds.o ${INCLUDE} ${LIBRARIES}

obj/trace.o: src/trace.c header/trace.h
	${CC} ${DEBUG} ${CFLAGS} src/trace.c -c -o obj/trace.o ${INCLUDE} ${LIBRARIES}

obj/reshape.o: src/reshape.c header/reshape.h
	${CC} ${DEBUG} ${CFLAGS} src/reshape.c -c -o obj/reshape.o ${INCLUDE} ${LIBRARIES}

obj/error.o: src/error.c header/error.h
	${CC} ${DEBUG} ${CFLAGS} src/error.c -c -o obj/error.o ${INCLUDE} ${LIBRARIES}

obj/siphash.o: src/siphash.c header/siphash.h
	${CC} ${DEBUG} ${CFLAGS} src/siphash.c -c -o obj/siphash.o ${INCLUDE} ${LIBRARIES}

obj/dye.o: src/dye.c header/dye.h
	${CC} ${DEBUG} ${CFLAGS} src/dye.c -c -o obj/dye.o ${INCLUDE} ${LIBRARIES}

## tests and housekeeping
.PHONY: test
test: test1_run test2_run

test1: libvltl.a tests/src/*.cc
	${CXX} ${DEBUG} ${CXXFLAGS} tests/src/test1.cc libvltl.a -o test1 ${INCLUDE} ${INCLUDE_TEST} ${LIBRARIES} ${LIBRARIES_TEST}

test1_run: test1
	./test1

# g++ must be used here!
test2: tests/fullpass/several_functions.bin.S tests/src/*.cc
	g++ ${DEBUG} ${CXXFLAGS} ${test_asm_files} tests/src/test2.cc -o test2 ${INCLUDE} ${INCLUDE_TEST} ${LIBRARIES} ${LIBRARIES_TEST}

test2_run: test2
	./test2

# kind of a misnomer to test the performance of a "test build" but it's comparative data
performance: test
	./test
	gprof ./test gmon.out

.PHONY: tags
tags:
	ctags -R .

.PHONY: clear
clear:
	clear
