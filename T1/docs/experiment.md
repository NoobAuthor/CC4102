# Experimento de Comparación: Mergesort vs Quicksort Externo

## Objetivo del Experimento

Este proyecto compara sistemáticamente el rendimiento de dos algoritmos de ordenamiento externo (**Mergesort** y **Quicksort**) bajo diferentes condiciones. El experimento mide:

- ⏱️ **Tiempo de ejecución promedio**
- 📁 **Accesos a disco** (lecturas y escrituras)
- 🔍 **Aridad óptima** para minimizar operaciones I/O

## Estructura del Experimento

### 1. Búsqueda Binaria de Aridad Óptima

El experimento determina automáticamente el mejor valor de aridad (`a`) para Mergesort usando un dataset de 60 millones de elementos:

```cpp
int findOptimalArity(...) {
    // Búsqueda binaria en rango [2, B/sizeof(int64_t)]
    while (low <= high) {
        int mid = (low + high)/2;
        externalMergesort(..., mid);  // Prueba aridad actual
        // Selecciona aridad con menos I/O
        if (total_io < min_io) best_arity = mid;
    }
    return best_arity;
}
```

**¿Por qué 60M elementos?**  
Es el tamaño máximo especificado en el enunciado para garantizar condiciones extremas.

### 2. Ejecución Comparativa

Para cada tamaño de dataset (4M a 60M elementos):

1. **Genera datos aleatorios**
2. **Ejecuta 5 veces cada algoritmo**
3. **Registra métricas** en `results.csv`

```cpp
for (auto k : multipliers) {
    size_t N = k * M / sizeof(int64_t);
    for (int i = 0; i < runs; ++i) {
        // Generar datos
        shuffle(data.begin(), data.end(), rng);

        // Ejecutar Quicksort
        externalQuicksort(..., best_arity);

        // Ejecutar Mergesort
        externalMergesort(..., best_arity);
    }
    // Calcular promedios
}
```

## Cómo Ejecutar el Experimento

### Requisitos Mínimos

- 8GB de RAM (para dataset de 60M)
- 5GB de espacio libre en disco
- Compilador C++17

### Pasos:

1. **Compilar el proyecto**:

   ```bash
   make clean && make
   ```

2. **Ejecutar con parámetros por defecto** (B=4KB, M=50MB):

   ```bash
   ./bin/experiment 4096 52428800
   ```

3. **Resultados**:
   - Se generará el archivo `results.csv`
   - Se mostrará la aridad óptima en consola
   ```
   Aridad óptima encontrada: 16
   [OK] Experimentación completada
   ```

### Parámetros Personalizables

| Parámetro | Descripción               | Valor por Defecto |
| --------- | ------------------------- | ----------------- |
| `B`       | Tamaño de bloque de disco | 4096 (4KB)        |
| `M`       | Memoria disponible        | 52428800 (50MB)   |

Ejemplo para 1MB de bloque y 100MB de memoria:

```bash
./bin/experiment 1048576 104857600
```

## Interpretación de Resultados

El archivo `results.csv` contiene:

```csv
N,alg,avg_time_ms,avg_reads,avg_writes
4000000,QUICK,1523.8,14200,13800
4000000,MERGE,1489.2,13500,13200
...
```

- **N**: Número de elementos
- **alg**: Algoritmo usado (QUICK/MERGE)
- **avg_time_ms**: Tiempo promedio en milisegundos
- **avg_reads/avg_writes**: Accesos a disco promedio

## Visualización de Datos

Ejemplo usando Python y Pandas:

```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('results.csv')
quicksort = df[df['alg'] == 'QUICK']
mergesort = df[df['alg'] == 'MERGE']

plt.plot(quicksort['N'], quicksort['avg_time_ms'], label='Quicksort')
plt.plot(mergesort['N'], mergesort['avg_time_ms'], label='Mergesort')
plt.xlabel('Tamaño de datos (N)')
plt.ylabel('Tiempo (ms)')
plt.legend()
plt.show()
```

## Consejos para Ejecución Exitosa

1. **Ejecutar en SSD**: Reduce tiempo de I/O un 300-500%
2. **Monitorear uso de disco**:
   ```bash
   watch -n 1 'df -h | grep experiment'
   ```
3. **Reducir dataset para pruebas**:
   ```cpp
   // En experiment.cpp:
   const vector<size_t> multipliers = {4, 8}; // Solo 4M y 8M
   ```

## Solución de Problemas

### Error: "No space left on device"

- Los archivos temporales consumen ~2x el tamaño original
- Solución: Liberar espacio o reducir tamaño máximo (`multipliers`)

### Resultados Inconsistentes

- Asegurar que el disco no esté siendo usado por otros procesos
- Ejecutar en máquina dedicada durante la experimentación
