CC := clang++
LDFLAGS=
# BASE_INC=networkplayground/include/networking
# BASE_INC=networkplaygroundclient/include/networking
BASE_INC=networkplayground/include/

# INCDIRS := $(addprefix -I,$(sort $(dir $(wildcard networkplayground/include/*/))))
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test

CC_FLAGS=-std=c++11 -I$(BASE_INC)
GTEST_INCLUDE=

# mark the main files so we can exclude them these during unit tests
CLIENT_MAIN=networkplaygroundclient/src/main.cpp
CLIENT_MAIN_OBJ=networkplaygroundclient/src/main.o
SERVER_MAIN=networkplaygroundserver/src/main.cpp
SERVER_MAIN_OBJ=networkplaygroundserver/src/main.o

NCDIR = networkplayground/include

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

%.o: %.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

clean:
	find . -name '*.o' -delete
	rm bin/

# clang++ -v -Inetworkplayground/include/ networkplayground/src/test.cpp