# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Werror -I./src
LDFLAGS := 

# Directory structure
BIN_DIR := bin
SRC_DIR := src
TEST_DIR := test
OBJ_DIR := obj

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
EXECUTABLES := $(BIN_DIR)/experiment $(BIN_DIR)/test_quicksort $(BIN_DIR)/test_mergesort

# Default target
all: dirs $(EXECUTABLES)

# Create necessary directories
dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

# Main experiment executable - SINGLE DEFINITION
$(BIN_DIR)/experiment: $(OBJ_DIR)/experiment.o $(OBJ_DIR)/external_mergesort.o $(OBJ_DIR)/external_quicksort.o $(OBJ_DIR)/disk_io.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Test executables
$(BIN_DIR)/test_quicksort: $(OBJ_DIR)/test_quicksort.o $(OBJ_DIR)/external_quicksort.o $(OBJ_DIR)/disk_io.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/test_mergesort: $(OBJ_DIR)/test_mergesort.o $(OBJ_DIR)/external_mergesort.o $(OBJ_DIR)/disk_io.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Pattern rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test targets
test: $(BIN_DIR)/test_quicksort $(BIN_DIR)/test_mergesort
	@echo "\n=== Running Tests ==="
	@$(BIN_DIR)/test_quicksort && echo "Quicksort test: PASS"
	@$(BIN_DIR)/test_mergesort && echo "Mergesort test: PASS"

# Experiment target
experiment: $(BIN_DIR)/experiment
	@echo "\n=== Running Experiment ==="
	@$(BIN_DIR)/experiment 4096 52428800  # B=4KB, M=50MB

# Docker target
docker:
	docker build -t external-sort .
	docker run --memory="512m" external-sort

# Clean target
clean:
	@rm -rf $(BIN_DIR) $(OBJ_DIR) *.bin test/*.bin results.csv
	@echo "Cleaned build artifacts and data files"

.PHONY: all dirs test experiment docker clean