CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Source files
SRCS = src/main.cpp src/kruskal.cpp src/union_find.cpp

# Object files
BUILD_DIR = build
OBJS = $(SRCS:src/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = kruskal

.PHONY: all clean experiment dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILD_DIR) input results

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) input/* results/*

experiment: all
	chmod +x run_experiment.sh
	./run_experiment.sh 