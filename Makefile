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

SRC  = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJ  = $(SRC:.cpp=.o)
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

format:
	astyle $(SRC)

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

TEST_SRC  = $(wildcard tests/*.cpp) $(wildcard tests/**/*.cpp)
TEST_OBJ  = $(TEST_SRC:.cpp=.o)

tests/%.o: tests/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(CATCH2_INC)

test_main: $(TEST_OBJ)
	$(CC) -o $(BIN)/test_execute_point $^ $(LDFLAGS) $(LIB) $(CATCH2_LIB)

test: export SPDLOG_LEVEL = debug
test: all catch2_lib test_main
	$(BIN)/test_execute_point

clean.s3dg:
	@echo Cleaning Binaries ...
	@rm -rf $(BIN) $(OBJ) $(TEST_OBJ) $(LIB) $(SRC:=.orig)
	@rm -rf build

clean: clean.s3dg
	@echo Cleaning Libraries ...
	@cd lib/glfw && make clean
	@cd lib/glew && make clean
	@cd lib/spdlog && make clean
	@cd lib/Catch2/build && make clean
