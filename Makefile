UNAME_S := $(shell uname -s)

CC = g++
CFLAGS = -std=c++14 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments
CFLAGS += -Ilib/glfw/include -Ilib/glew/include -Ilib/spdlog/include
CFLAGS += -Iinclude

LDFLAGS += lib/glfw/src/libglfw3.a lib/glew/lib/libGLEW.a lib/spdlog/libspdlog.a

ifeq ($(UNAME_S), Darwin)
	LDFLAGS += -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa
endif

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -ldl -lpthread -lGL
endif

INC_HPP = $(wildcard include/*.hpp) $(wildcard include/**/*.hpp) $(wildcard include/**/**/*.hpp)
INC  = $(wildcard include/*.h) $(wildcard include/**/*.h) $(wildcard include/**/**/*.h) $(INC_HPP)
SRC  = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)
OBJ = $(SRC:.cpp=.o)
LIB = $(filter-out src/main.o, $(OBJ))

BIN = bin

.PHONY: all clean test

all: dirs exlibs main

exlibs:
	cd lib/glfw && cmake . && make
	cd lib/glew && make
	cd lib/spdlog && cmake . && make

dirs:
	mkdir -p ./$(BIN)

run: all
	$(BIN)/main

main: $(OBJ)
	$(CC) -o $(BIN)/main $^ $(LDFLAGS)

src/%.o: src/%.cpp 
	$(CC) -o $@ -c $< $(CFLAGS)


# Testing

CATCH2_SRC = lib/Catch2
CATCH2_BUILD_SRC = $(CATCH2_SRC)/build/src
CATCH2_LIB = $(CATCH2_BUILD_SRC)/libCatch2Main.a $(CATCH2_BUILD_SRC)/libCatch2.a
CATCH2_INC = -I$(CATCH2_SRC)/src -I$(CATCH2_SRC)/build/generated-includes

catch2_lib:
	cd lib/Catch2 &&\
	cmake -DCMAKE_CXX_COMPILER=g++ -Bbuild &&\
	cd build &&\
	make

TEST_SRC  = $(wildcard tests/*.cpp) $(wildcard tests/**/*.cpp)  $(wildcard tests/**/**/*.cpp)
TEST_OBJ  = $(TEST_SRC:.cpp=.o)

tests/%.o: tests/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(CATCH2_INC)

test_main: $(TEST_OBJ)
	$(CC) -o $(BIN)/test_execute_point $^ $(LIB) $(CATCH2_LIB) $(LDFLAGS)

test: export SPDLOG_LEVEL = debug
test: all catch2_lib test_main
	$(BIN)/test_execute_point $(ARGS)

clean.s3dg.tests:
	@echo Cleaning Test Binaries ...
	@rm -rf $(TEST_OBJ)

clean.s3dg: clean.s3dg.tests
	@echo Cleaning Binaries ...
	@rm -rf $(BIN) $(OBJ) $(TEST_OBJ) $(LIB)
	@rm -rf build

clean: clean.s3dg
	@echo Cleaning Libraries ...
	@cd lib/glfw && make clean
	@cd lib/glew && make clean
	@cd lib/spdlog && make clean
	@cd lib/Catch2/build && make clean

FORMATTED_FILES = $(SRC) $(TEST_SRC) $(INC)

format:
	@astyle $(FORMATTED_FILES) \
	--style=google \
	--indent=spaces=4 \
	--indent-classes \
	--indent-switches \
	--indent-cases \
	--indent-preproc-block \
	--indent-preproc-define \
	--indent-col1-comments \
	--indent-namespaces \
	--min-conditional-indent=0 \
	--max-instatement-indent=120 \
	--break-blocks \
	--pad-oper \
	--pad-header \
	--unpad-paren \
	--delete-empty-lines \
	--align-pointer=type \
	--align-reference=type \
	--add-brackets \
	--convert-tabs \
	--max-code-length=200 \
	--break-after-logical \
	--lineend=linux \
	--formatted
