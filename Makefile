CC := clang++
LDFLAGS=
INC_DIR = networkplayground/include/networking
CC_FLAGS=-std=c++11 -I$(INC_DIR)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test

# CLIENT_CODE=

NCDIR = networkplayground/include

# CPP_FILES := $(wildcard networkplayground/**/*/*.cpp) $(wildcard networkplayground/*/*.cpp)
CPP_FILES := $(shell find networkplayground -name '*.cpp')
CPP_CLIENT := $(wildcard networkplaygroundclient/*/*.cpp)

# BASE_OBJ_FILES := $(addprefix obj/base/,$(notdir $(CPP_FILES:.cpp=.o)))
BASE_OBJ_FILES := $(patsubst %.cpp,%.o,$(CPP_FILES))
CLIENT_OBJ_FILES := $(addprefix obj/client/,$(notdir $(CPP_CLIENT:.cpp=.o)))

# .SUFFIXES: .o .h .c .hpp .cpp


# base: $(BASE_OBJ_FILES)
# 	@echo "Building base files |$(BASE_OBJ_FILES)|"
# 	@echo "Building CPP files |$(CPP_FILES)|"
# 	@echo "Running $@ $^|"
# 	CC $(LD_FLAGS) -o $@ $^

client: $(BASE_OBJ_FILES) $(CLIENT_OBJ_FILES)
	@echo "|$(BASE_OBJ_FILES)|"
	@echo "|$(CLIENT_OBJ_FILES)|"
	@echo "|$@ $^|"
	$(CC)  $^ -o $@

obj/client/%.o: networkplaygroundclient/*/%.cpp
	@echo "Building Client object files $@ to $<"
	@echo ""
	$(CC) $(CC_FLAGS) -c -o $@ $<

# Get top level cpp files
%.o: %.cpp
	@echo "Building base object files $@ to $<"
	@echo ""
	$(CC) $(CC_FLAGS) -c -o $@ $<
# Get any subdirectory cpp files
# obj/base/%.o: networkplayground/src/%.cpp
# 	@echo "Building base object files $@ to $<"
# 	@echo ""
# 	$(CC) $(CC_FLAGS) -c -o $@ $<
# Get any subdirectory cpp files
# obj/base/%.o: networkplayground/**/*/%.cpp
# 	@echo "Building base object files $@ to $<"
# 	@echo ""
# 	$(CC) $(CC_FLAGS) -c -o $@ $<


# obj/client/%.o: $(CPP_CLIENT)
#   @echo "Now Building $@ to $<"
#   CC $(CC_FLAGS) -c -o $@ $<

clean:
	rm obj/base/*
	rm obj/client/*
	rm client

# clang++ -v -Inetworkplayground/include/ networkplayground/src/test.cpp