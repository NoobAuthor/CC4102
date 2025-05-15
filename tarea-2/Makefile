# Makefile for Tarea 2 - Algoritmos y Estructuras de Datos

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin
TEST_DIR = tests

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/main

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean test

test:
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_main.cpp -o $(BIN_DIR)/test_main && $(BIN_DIR)/test_main 