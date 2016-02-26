CC := clang++
LDFLAGS=
# BASE_INC_FLAG=networkplayground/include/networking
# BASE_INC_FLAG=networkplaygroundclient/include/networking
BASE_INC_FLAG=-Inetworkplayground/include/

EXECUTABLE=test

CC_FLAGS=-std=c++11 $(BASE_INC_FLAG)

# mark the main files so we can exclude them these during unit tests
CLIENT_MAIN=networkplaygroundclient/src/main.cpp
CLIENT_MAIN_OBJ=networkplaygroundclient/src/main.o
SERVER_MAIN=networkplaygroundserver/src/main.cpp
SERVER_MAIN_OBJ=networkplaygroundserver/src/main.o

# Test data
GTEST_INCLUDE=/opt/gtest/googletest/include/
GTEST_LIB=/opt/gtest/lib/gtest_main.a
TESTS_MAIN=unit_tests/main.cpp
TESTS_MAIN_OBJ=unit_tests/main.o
TEST_FLAGS=-std=c++11 -I $(GTEST_INCLUDE) -pthread $(GTEST_LIB)

CPP_BASE := $(shell find networkplayground -name '*.cpp')
# find all cpp files but exclude main.cpp, as we only want to compile the main
# when running the client
CPP_CLIENT := $(shell find networkplaygroundclient ! -path "$(CLIENT_MAIN)" -name '*.cpp')
CPP_SERVER := $(shell find networkplaygroundserver ! -path "$(SERVER_MAIN)" -name '*.cpp')

# BASE_OBJ_FILES := $(addprefix obj/base/,$(notdir $(CPP_BASE:.cpp=.o)))
BASE_OBJ_FILES := $(patsubst %.cpp,%.o,$(CPP_BASE))
CLIENT_OBJ_FILES := $(patsubst %.cpp,%.o,$(CPP_CLIENT))

# .SUFFIXES: .o .h .c .hpp .cpp

client: $(BASE_OBJ_FILES) $(CLIENT_OBJ_FILES) $(CLIENT_MAIN_OBJ)
	$(CC) $^ -o bin/$@

server: $(BASE_OBJ_FILES) $(CLIENT_OBJ_FILES) $(CLIENT_MAIN_OBJ)
	$(CC) $^ -o bin/$@

tests: $(BASE_OBJ_FILES) $(CLIENT_OBJ_FILES) $(TESTS_MAIN_OBJ)
	$(CC) $(BASE_INC_FLAG) $(TEST_FLAGS) $^ -o bin/$@

# TODO: Make this better?
unit_tests/main.o: unit_tests/main.cpp
	$(CC) $(BASE_INC_FLAG) $(TEST_FLAGS) -c -o $@ $<

%.o: %.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

clean:
	find . -name '*.o' -delete
	rm bin/*

# clang++ -v -Inetworkplayground/include/ networkplayground/src/test.cpp