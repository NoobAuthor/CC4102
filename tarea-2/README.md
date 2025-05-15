# Tarea 2 - Algoritmos y Estructuras de Datos

## Descripción

Este proyecto implementa el algoritmo de Kruskal para encontrar el Árbol de Expansión Mínima (MST) de un grafo no dirigido y ponderado, utilizando la estructura de datos Union-Find (conjuntos disjuntos). Además, automatiza la visualización y el análisis de resultados mediante scripts en Python y un sistema de experimentación reproducible.

### ¿Qué se hizo?

- **Implementación en C++** del algoritmo de Kruskal y la estructura Union-Find.
- **Automatización de experimentos** y visualización de resultados usando Python y Makefile.
- **Generación de informes automáticos** con visualizaciones y resúmenes de los experimentos.
- **Separación clara de responsabilidades**: C++ para el algoritmo y Python para la visualización y análisis.

### ¿Por qué se hizo así?

- **Eficiencia y claridad**: C++ es ideal para algoritmos eficientes y control de bajo nivel.
- **Visualización poderosa**: Python (con NetworkX y Matplotlib) permite crear gráficos técnicos y comparativos de alta calidad.
- **Reproducibilidad y limpieza**: El uso de Makefile permite automatizar todo el flujo de trabajo, limpiar resultados y generar informes de manera sencilla y reproducible.
- **Organización de resultados**: Todos los resultados y salidas se guardan en la carpeta `results/` para facilitar la revisión y el análisis.

## Estructura del Proyecto

```
├── src/
│   ├── main.cpp           # Lógica principal (Kruskal, Union-Find)
│   ├── union_find.h/cpp   # Estructura Union-Find
│   └── graph/graph.py     # Visualización y análisis en Python
├── tests/
│   └── test_main.cpp      # Pruebas unitarias de Union-Find
├── results/               # Resultados de los experimentos
├── input.txt              # Archivo de entrada para el experimento
├── Makefile               # Automatización de compilación y experimentos
├── README.md              # Este archivo
```

## Compilación y Dependencias

- **C++17** (g++ recomendado)
- **Python 3** con `networkx` y `matplotlib`

Para instalar las dependencias de Python:

```sh
make python-reqs
```

Para compilar el proyecto:

```sh
make
```

## Uso y Automatización

### Ejecutar un experimento completo

```sh
make experiment
```

Esto:

- Genera automáticamente el archivo `input.txt` con un grafo robusto.
- Compila y ejecuta el algoritmo de Kruskal en C++.
- Guarda todos los resultados en la carpeta `results/`.
- Ejecuta el script de Python para visualizar y analizar el MST.

### Limpiar todos los resultados y archivos generados

```sh
make clean
```

### Ejecutar pruebas unitarias

```sh
make test
```

### Generar informe automático

```sh
make inform
```

Esto crea un archivo `results/informe.md` con el resumen, visualizaciones y enlaces a los datos del experimento.

## Resultados y Salidas

- **results/mst_output.txt**: Aristas y peso total del MST.
- **results/summary.txt**: Resumen del experimento (nodos, aristas, peso total, etc).
- **results/mst_edges.csv**: MST en formato CSV.
- **results/mst_visualization.png**: Imagen del MST.
- **results/comparison.png**: Comparación lado a lado del grafo original y el MST.
- **results/informe.md**: Informe automático en Markdown.

## Diseño y Decisiones Técnicas

- **Separación de lenguajes**: C++ para el algoritmo (eficiencia), Python para visualización (flexibilidad).
- **Automatización**: Makefile orquesta todo el flujo, desde la generación de entradas hasta la limpieza y el informe.
- **Reproducibilidad**: Cada experimento es completamente reproducible y sus resultados quedan organizados.
- **Extensibilidad**: Es fácil agregar más experimentos, visualizaciones o análisis.

## Autores

- Igor Assis
- Nicolás Montenegro
- Joaquín Ramírez
