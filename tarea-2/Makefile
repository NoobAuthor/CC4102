# Makefile for Tarea 2 - Algoritmos y Estructuras de Datos

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin
TEST_DIR = tests
PY_GRAPH = $(SRC_DIR)/graph/graph.py
PY_REQS = $(SRC_DIR)/graph/requirements.txt
RESULTS_DIR = results

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
	rm -rf $(OBJ_DIR) $(BIN_DIR) results input.txt

.PHONY: all clean test run python-reqs experiment inform

test:
	$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_main.cpp src/union_find.cpp -o $(BIN_DIR)/test_main && $(BIN_DIR)/test_main

python-reqs:
	pip install -r $(PY_REQS)

run: all
	@mkdir -p $(RESULTS_DIR)
	@echo "Ingrese el grafo por stdin: n m y luego cada arista (u v w)"
	$(TARGET) $(RESULTS_DIR) < input.txt
	python3 $(PY_GRAPH) $(RESULTS_DIR)

experiment: all python-reqs
	@mkdir -p $(RESULTS_DIR)
	@echo "[EXPERIMENT] Creando archivos de entrada para el experimento..."
	@echo "6 9" > input.txt
	@echo "0 1 4" >> input.txt
	@echo "0 2 4" >> input.txt
	@echo "1 2 2" >> input.txt
	@echo "1 0 4" >> input.txt
	@echo "2 0 4" >> input.txt
	@echo "2 1 2" >> input.txt
	@echo "2 3 3" >> input.txt
	@echo "2 5 2" >> input.txt
	@echo "2 4 4" >> input.txt
	@echo "3 4 3" >> input.txt
	@echo "4 5 3" >> input.txt
	@echo "5 3 1" >> input.txt
	@echo "[EXPERIMENT] Compilando el proyecto..."
	$(TARGET) $(RESULTS_DIR) < input.txt
	@echo "[EXPERIMENT] Ejecutando visualización en Python..."
	python3 $(PY_GRAPH) $(RESULTS_DIR)
	@echo "[EXPERIMENT] Experimento completado correctamente."

inform:
	@mkdir -p $(RESULTS_DIR)
	@echo "# Informe de Experimento de Árbol de Expansión Mínima" > $(RESULTS_DIR)/informe.md
	@echo "" >> $(RESULTS_DIR)/informe.md
	@echo "## Resumen" >> $(RESULTS_DIR)/informe.md
	@cat $(RESULTS_DIR)/summary.txt >> $(RESULTS_DIR)/informe.md
	@echo "" >> $(RESULTS_DIR)/informe.md
	@echo "## Visualización del MST" >> $(RESULTS_DIR)/informe.md
	@echo "![](mst_visualization.png)" >> $(RESULTS_DIR)/informe.md
	@echo "" >> $(RESULTS_DIR)/informe.md
	@echo "## Comparación Grafo Original vs MST" >> $(RESULTS_DIR)/informe.md
	@echo "![](comparison.png)" >> $(RESULTS_DIR)/informe.md
	@echo "" >> $(RESULTS_DIR)/informe.md
	@echo "## MST en formato CSV" >> $(RESULTS_DIR)/informe.md
	@echo "Archivo: [mst_edges.csv](mst_edges.csv)" >> $(RESULTS_DIR)/informe.md
	@echo "" >> $(RESULTS_DIR)/informe.md
	@echo "---" >> $(RESULTS_DIR)/informe.md
	@echo "Informe generado automáticamente a partir de los resultados del experimento." >> $(RESULTS_DIR)/informe.md 