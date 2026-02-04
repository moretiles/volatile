CFLAGS=-Wall -Wextra -Wpedantic --std=c11 -fwrapv
CXXFLAGS=-Wall -Wextra -Wpedantic --std=gnu++20 -fwrapv -Wno-missing-field-initializers
DEBUG=-g3 -ggdb -D__VLT_EXECUTION_DEBUG__=1
OPTIMIZE=-O0

INCLUDE=-I ./header/ -I ./src/
INCLUDE_TEST=
LIBRARIES=
LIBRARIES_TEST=-lgtest -lgtest_main

ANALYZE_GCC=-fanalyzer
ANALYZE_CLANG=-analyze-headers

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
	${CC} ${OPTIMIZE} ${CFLAGS} src/core.c -o vltl ${INCLUDE} ${LIBRARIES}

libvltl.a: obj/asm.o obj/ast.o obj/compile.o obj/debug.o obj/isa.o obj/lang.o obj/global.o obj/lexer.o obj/sast.o obj/convert.o obj/ds.o
	ar rcs libvltl.a obj/*.o

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

## tests and housekeeping
test: libvltl.a tests/src/*.cc
	${CXX} ${DEBUG} ${CXXFLAGS} tests/src/main.cc libvltl.a -o test ${INCLUDE} ${INCLUDE_TEST} ${LIBRARIES} ${LIBRARIES_TEST}

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
