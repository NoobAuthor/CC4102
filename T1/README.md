# Tarea 1: Comparación de Mergesort Externo vs Quicksort Externo

## Introducción

Este proyecto implementa y compara dos algoritmos de ordenamiento externo (**Mergesort Externo** y **Quicksort Externo**) diseñados para trabajar con conjuntos de datos que no caben completamente en la memoria principal. El objetivo es evaluar su rendimiento en términos de:

- **Tiempo de ejecución**
- **Cantidad de accesos a disco**
- **Aridad óptima** (parámetro crítico para Mergesort)

Ambos algoritmos utilizan la misma aridad óptima (`a`), determinada mediante una búsqueda binaria sobre un dataset de 60M elementos.

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

3. **Ejecutar experimento completo**:

   ```bash
   make experiment  # Genera resultados en results.csv
   ```

   Este comando realiza **3 pasos automáticos**:

   - **Paso 1**: Genera un dataset de 60M elementos y determina la aridad óptima mediante búsqueda binaria.
   - **Paso 2**: Ejecuta 5 iteraciones para cada tamaño de dataset (4M a 60M).
   - **Paso 3**: Guarda métricas promediadas en `results.csv`.

4. **Opcional: Ejecutar en Docker** (limita memoria a 512MB):
   ```bash
   make docker   # Construye y ejecuta en contenedor
   ```

### Parámetros clave:

| Parámetro | Valor por defecto | Descripción                                 |
| --------- | ----------------- | ------------------------------------------- |
| `B`       | 4096 (4KB)        | Tamaño de bloque de disco                   |
| `M`       | 52428800 (50MB)   | Memoria disponible                          |
| `a`       | Automático        | Aridad óptima determinada experimentalmente |

### Salidas:

1. **Resultados numéricos**:
   ```csv
   N,alg,avg_time_ms,avg_reads,avg_writes
   4000000,QUICK,1523.8,14200,13800
   4000000,MERGE,1489.2,13500,13200
   ...
   ```
2. **Log de consola**:
   ```text
   Aridad óptima encontrada: 16
   Ejecutando experimento para N=4,000,000...
   ```

## Personalización avanzada

1. **Modificar parámetros base** (en `Makefile`):

   ```makefile
   B := 4096        # Tamaño de bloque (bytes)
   M := 52428800    # Memoria disponible (bytes)
   ```

2. **Forzar una aridad específica** (modificar `experiment.cpp`):
   ```cpp
   // En main(), reemplazar:
   const int best_arity = findOptimalArity(...);
   // Por:
   const int best_arity = 24; // Valor manual
   ```

## Estructura del proyecto

```
T1/
├── bin/           # Ejecutables compilados
├── doc/           # Documentation on each individual file
├── src/           # Código fuente
├── test/          # Pruebas unitarias
├── Makefile       # Configuración de compilación
├── results.csv    # Resultados del experimento (generado)
└── README.md      # Esta documentación
```

## Análisis de resultados

El archivo `results.csv` permite comparar:

- **Escalabilidad** de ambos algoritmos con diferentes `N`.
- **Impacto de la aridad** en el rendimiento.
- **Eficiencia en I/O** (lecturas/escrituras por bloque).

¡Contribuciones y mejoras son bienvenidas! 🚀
