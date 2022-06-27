CC = g++
CFLAGS = -std=c++14 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments
CFLAGS += -Ilib/glfw/include -Ilib/glew/include
CFLAGS += -Iinclude

LDFLAGS = -ldl -lpthread -lGL 
LDFLAGS += lib/glfw/src/libglfw3.a lib/glew/lib/libGLEW.a

SRC  = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJ  = $(SRC:.cpp=.o)
LIB = $(filter-out src/main.o, $(OBJ))

BIN = bin

.PHONY: all clean lib

all: dirs exlibs main

exlibs:
	cd lib/glfw && cmake . && make
	cd lib/glew && make

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

USR_BIN = /usr/local/lib
CATCH2_BINARIES = $(USR_BIN)/libCatch2Main.a $(USR_BIN)/libCatch2.a

TEST_SRC  = $(wildcard tests/*.cpp) $(wildcard tests/**/*.cpp)
TEST_OBJ  = $(TEST_SRC:.cpp=.o)

tests/%.o: tests/%.cpp lib
	$(CC) -o $@ $< $(CATCH2_BINARIES) $(CFLAGS) $(LIB)

test: all $(TEST_OBJ)
	for file in $^ ; do \
		$${file}; \
	done

clean.s3dg:
	@echo Cleaning Binaries ...
	@rm -rf $(BIN) $(OBJ) $(TEST_OBJ) $(LIB) $(SRC:=.orig)
	@rm -rf build

clean: clean.s3dg
	@echo Cleaning Binaries ...
	@rm -rf $(BIN) $(OBJ) $(TEST_OBJ) $(LIB) $(SRC:=.orig)
	@rm -rf build
	@echo Cleaning Libraries ...
	@cd lib/glfw && make clean
	@cd lib/glew && make clean
