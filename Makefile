TARGET = criti

# CC_SLAVE = sw5cc -slave
# CC_HOST = sw5cc -host
# CXX_HOST = sw5CC -host
# LINK = sw5CC -hybrid
CC = clang
CXX = clang++
LINK = clang++

C_FLAGS = -Wall -Wextra -pedantic
CXX_FLAGS = -Wall -Wextra -pedantic
IGNORED_WARNINGS = -Wno-pointer-arith -Wno-newline-eof
OPT_LEVEL = -O0

C_SRC = $(shell ls src/*.c)
CPP_SRC = $(shell ls src/*.cpp)
BASE = $(basename $(C_SRC) $(CPP_SRC))
OBJS = $(addsuffix .o, $(BASE))

$(TARGET): $(OBJS)
	$(LINK) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(OPT_LEVEL) $(C_FLAGS) $(IGNORED_WARNINGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(OPT_LEVEL) $(CXX_FLAGS) $(IGNORED_WARNINGS) -c $< -o $@

.PHONY: clean
clean:
	-rm -rf $(TARGET) $(OBJS)