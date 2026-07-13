CXX ?= g++
CXXFLAGS ?= -std=c++20 -O3 -Wall -Wextra -flto -pthread
LDFLAGS ?= -flto -pthread

SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)
TARGET := knife

.PHONY: all clean debug perft

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

debug: CXXFLAGS = -std=c++20 -O0 -g -Wall -Wextra -pthread
debug: LDFLAGS = -pthread
debug: clean $(TARGET)

perft: $(TARGET)
	@echo position startpos > perft_cmd.txt
	@echo perft 5 >> perft_cmd.txt
	@echo quit >> perft_cmd.txt
	@./knife < perft_cmd.txt
	@rm -f perft_cmd.txt

clean:
	rm -f $(OBJS) $(TARGET)
