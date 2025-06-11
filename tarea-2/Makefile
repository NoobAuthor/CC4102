CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Main program
SRCS = src/main.cpp src/kruskal.cpp src/union_find.cpp
TARGET = kruskal

# Test program
TEST_SRCS = tests/test_main.cpp src/kruskal.cpp src/union_find.cpp
TEST_TARGET = test_main

.PHONY: all clean test experiment

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

test: $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) -o $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf input/* results/*

experiment: all
	mkdir -p input results
	chmod +x run_experiment.sh
	./run_experiment.sh 