# Algoritmos de Ordenamiento Externos

Este proyecto implementa y compara dos algoritmos de ordenamiento externo: MergeSort y QuickSort. Incluye funcionalidad para medir tiempos de ejecución y accesos a disco durante las operaciones de ordenamiento.

## Requisitos

- CMake (versión 3.10 o superior)
- Compilador C++ compatible con C++17
- Python 3 con las siguientes bibliotecas:
  - matplotlib
  - pandas
  - numpy
- Docker (opcional, para pruebas con memoria limitada)

## Instalación de Dependencias

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake python3 python3-pip
pip3 install matplotlib pandas numpy
```

### Fedora

```bash
sudo dnf install gcc-c++ cmake python3 python3-pip
pip3 install matplotlib pandas numpy
```

### macOS

```bash
brew install cmake python
pip3 install matplotlib pandas numpy
```

### Windows

- Instalar CMake
- Instalar MinGW o Visual Studio
- Instalar Python
- Instalar las bibliotecas de Python:

```bash
pip install matplotlib numpy pandas
```

## Compilación del Proyecto

- Clonar el repositório:

```bash
git clone <url-del-repositorio>
cd T1
```

- Crear un directorio de compilación:

```bash
mkdir build
cd build
```

- Configurar el proyecto con CMake:

```bash
cmake ..
```

- Compilar el proyecto:

```bash
make
```

## Ejecución de Experimentos

El proyecto incluye varios targets de CMake para facilitar la ejecución de experimentos:
Generación de Datos

- Generar todos los conjuntos de datos (4M hasta 60M):

```bash
make generate_data
```

- Generar datos hasta un tamaño específico (ejemplo: 32M):

```bash
make generate_data_32M
```

Encontrar la Aridad Óptima
La aridad óptima se utiliza tanto para MergeSort como para QuickSort:

```bash
make find_optimal_arity
```

Ejecutar Experimentos

- Ejecutar experimentos para todos los tamaños:

```bash
make run_experiments_all
```

- Ejecutar experimentos para un tamaño específico (ejemplo: 32M):

```bash
make run_experiments_32M
```

- Generar visualizaciones de resultados:

```bash
make visualize_results
```

- Ejecutar el flujo completo (generación de datos, búsqueda de aridad, experimentos, visualizaciones):

```bash
make run_complete_experiment
```

## Uso del Contenedor Docker

Para una ejecución con memoria limitada controlada (requisito del proyecto):

- Instalar Docker desde docker.com

- Verificar la instalación:

```bash
docker --version
```

- Si aparece un error de permisos, añadir tu usuario al grupo docker:

```bash
sudo usermod -aG docker $USER
newgrp docker  # Aplica los cambios sin cerrar sesión
```

- Ejecutar el contenedor con el proyecto montado:

```bash
docker run --rm -it -v "$PWD":/workspace pabloskewes/cc4102-cpp-env bash
```

- Dentro del contenedor, instalar CMake si es necesario:

```bash
apt-get update
apt-get install -y cmake python3-dev python3-pip
pip3 install pandas matplotlib numpy
```

- Compilar y ejecutar dentro del contenedor:

```bash
cd /workspace
mkdir -p build
cd build
cmake ..
make run_complete_experiment
```

- Si el contenedor se queda sin memoria, aumentar el límite:

```bash
docker run --rm -it -m 70m -v "$PWD":/workspace pabloskewes/cc4102-cpp-env bash
```

## Estructura del Proyecto

- include/: Archivos de cabecera

  - algorithms/: Declaraciones de los algoritmos de ordenamiento
  - utils/: Utilidades (manejo de archivos, temporizador, generación de datos)

- src/: Implementaciones

  - algorithms/: Implementación de los algoritmos
  - utils/: Implementación de utilidades

- tests/: Pruebas y experimentos

- build/: Directorio generado para archivos compilados
  - data/: Datos generados y resultados
  - results/: Gráficos generados

## Limpieza

Para limpiar datos y resultados generados:

```bash
make clean_data
make clean_results
```

## Notas Adicionales

- El parámetro M está configurado en 50MB tal como se requiere en el proyecto
- La búsqueda de aridad óptima explora el rango [2, 512]
- Los experimentos generan 5 secuencias para cada tamaño de datos
- Las visualizaciones muestran tiempos de ejecución y accesos a disco para ambos algoritmos
