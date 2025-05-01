### Fase 1: Creación de Runs Iniciales

1. El archivo de entrada se divide en chunks (fragmentos) que caben en la memoria disponible.
2. Cada chunk se carga en memoria, se ordena utilizando un algoritmo de ordenamiento interno (en este caso, `std::sort`), y se escribe de vuelta al disco como un "run" ordenado.
3. Este proceso continúa hasta que todo el archivo de entrada ha sido procesado, generando múltiples runs ordenados.

### Fase 2: Merge de Runs

1. Los runs creados en la fase anterior se combinan de manera eficiente usando un proceso de merge de K-vías, donde K es la aridad del merge.
2. Si hay más runs que la aridad permitida, se realizan múltiples pasadas de merge, combinando runs en cada pasada hasta que quede un solo run final ordenado.
3. En cada paso del merge, se mantienen buffers para cada run de entrada y para el run de salida para minimizar los accesos a disco.

## Componentes Principales

### `external_mergesort`

Función principal que coordina todo el proceso de ordenamiento externo:

1. Determina el número de enteros que pueden caber en un bloque y en memoria.
2. Crea archivos temporales para los runs.
3. Llama a `create_initial_runs` para generar los runs iniciales ordenados.
4. Realiza pasadas sucesivas de merge usando `merge_runs` hasta que quede un solo run.
5. Renombra el último run como el archivo de salida final y limpia los archivos temporales.

```cpp
void external_mergesort(const char* input_file, const char* output_file,
                        size_t size, size_t block_size,
                        size_t memory_limit, int arity)
```

### `create_initial_runs`

Divide el archivo de entrada en fragmentos que caben en memoria, los ordena y los escribe como runs individuales:

1. Lee bloques del archivo de entrada hasta llenar la memoria disponible.
2. Ordena los datos en memoria usando `std::sort`.
3. Escribe los datos ordenados como un run en un archivo temporal.
4. Repite hasta procesar todo el archivo de entrada.

```cpp
int create_initial_runs(const char* input_file, char** run_files,
                        size_t size, size_t block_size,
                        size_t memory_limit)
```

### `merge_runs`

Combina múltiples runs en uno solo utilizando un algoritmo de merge de K-vías:

1. Abre los archivos de cada run para lectura.
2. Asigna buffers para cada run de entrada y para el run de salida.
3. Inicializa estructuras para rastrear posiciones y elementos restantes en cada run.
4. Carga los primeros bloques de cada run en sus respectivos buffers.
5. Repite hasta que todos los runs estén vacíos:
   - Encuentra el run con el elemento más pequeño actual.
   - Mueve ese elemento al buffer de salida.
   - Si el buffer de salida está lleno, escríbelo a disco.
   - Si un buffer de entrada se vacía y quedan elementos en el run, carga más datos de ese run.
6. Escribe cualquier dato restante en el buffer de salida y limpia recursos.

```cpp
void merge_runs(char** run_files, size_t* run_sizes,
                const char* output_file, int num_runs,
                size_t block_size, size_t buffer_blocks,
                int arity)
```

## Manejo de Disco

El código implementa un manejo eficiente de disco mediante:

1. **Operaciones por Bloques**: Todas las lecturas y escrituras se realizan por bloques completos.
2. **Buffers**: Se utilizan buffers en memoria para minimizar las operaciones de E/S.
3. **Archivos Temporales**: Los runs intermedios se almacenan en archivos temporales que se eliminan cuando ya no son necesarios.

## Función de Prueba

La función `test_mergesort` permite probar el algoritmo con diferentes parámetros y medir su rendimiento:

1. Crea una copia del archivo de entrada para no modificarlo.
2. Ejecuta el algoritmo de mergesort y mide el tiempo y los accesos a disco.
3. Verifica que el resultado esté correctamente ordenado.
4. Devuelve una métrica combinada de rendimiento (IO + tiempo).

```cpp
int64_t test_mergesort(const char* filename, size_t size,
                      size_t block_size, size_t memory_limit,
                      int arity)
```

## Complejidad

La complejidad del algoritmo en términos de accesos a disco (I/O) es:

O((N/B) \* log_K(N/M))

Donde:

- N: Número total de enteros a ordenar
- B: Número de enteros por bloque
- M: Número de enteros que caben en memoria
- K: Aridad del merge

Esta implementación está optimizada para minimizar tanto los accesos a disco como el tiempo total de ejecución.
