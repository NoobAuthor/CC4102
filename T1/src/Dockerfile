# Usa una imagen base de Ubuntu con herramientas de desarrollo
FROM ubuntu:20.04

# Instala las dependencias necesarias (G++, make, y otros)
RUN apt-get update && \
    apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Establece el directorio de trabajo dentro del contenedor
WORKDIR /app

# Copia todo el código fuente al contenedor
COPY . /app

# Compila el código C++ en el contenedor
RUN g++ -o external_mergesort external_mergesort.cpp

# Define el comando predeterminado que ejecutará el contenedor
CMD ["./external_mergesort"]
