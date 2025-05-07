# Tarea 1: Comparación de Mergesort Externo vs Quicksort Externo

## Introducción

Este proyecto implementa y compara dos algoritmos de ordenamiento externo (**Mergesort Externo** y **Quicksort Externo**) diseñados para trabajar con conjuntos de datos que no caben completamente en la memoria principal. El objetivo es evaluar su rendimiento en términos de:

- **Tiempo de ejecución**
- **Cantidad de accesos a disco**  
  Ambos factores críticos en aplicaciones de big data y procesamiento de archivos masivos.

## Objetivos

1. Implementar los algoritmos de Mergesort y Quicksort adaptados a memoria secundaria.
2. Comparar su eficiencia usando métricas de tiempo y operaciones de I/O.
3. Determinar bajo qué condiciones cada algoritmo es más adecuado.

## Estructura del Código

### Componentes principales:

1. **`external_mergesort`**

   - Divide el archivo en _runs_ ordenados.
   - Realiza merges en cascada con una aridad óptima (`a`).

2. **`external_quicksort`**

   - Particiona recursivamente el archivo usando pivotes seleccionados con _reservoir sampling_.
   - Ordena particiones en memoria cuando es posible.

3. **`disk_io`**
   - Módulo de lectura/escritura en bloques de tamaño `B`.
   - Cuenta accesos a disco para métricas.

### Archivos clave:

- `src/`: Implementaciones de los algoritmos y E/S.
- `test/`: Pruebas unitarias de correctitud.
- `experiment.cpp`: Script de experimentación automatizada.

## Instrucciones de Ejecución

### Requisitos:

- Compilador C++17 (g++ o clang++)
- Docker (opcional, para entorno controlado)

### Pasos:

1. **Compilar el proyecto**:

   ```bash
   make          # Compila todos los componentes
   ```

2. **Ejecutar pruebas de correctitud**:

   ```bash
   make tests    # Verifica que ambos algoritmos ordenen correctamente
   ```

3. **Ejecutar experimento principal**:

   ```bash
   make experiment  # Genera resultados en results.csv (B=4KB, M=50MB)
   ```

4. **Opcional: Ejecutar en Docker** (limita memoria a 512MB):
   ```bash
   make docker   # Construye y ejecuta en contenedor
   ```

### Parámetros personalizados:

Modifique en el `Makefile`:

```makefile
# Tamaño de bloque (B) y memoria (M)
B := 4096        # 4KB
M := 52428800    # 50MB
```

## Resultados

El experimento genera un archivo `results.csv` con formato:

```csv
N,algorithm,avg_time_ms,avg_reads,avg_writes
```

## Conclusión

Este proyecto permite analizar cómo escalan ambos algoritmos con diferentes tamaños de datos (`N`), ayudando a elegir la mejor opción según las restricciones de memoria y requisitos de rendimiento.
