# Renderizador de Imágenes C++ (AoS)

Este proyecto de la asignatura Arquitectura de Computadores implementa un sistema de procesamiento y renderizado de imágenes en C++20 optimizado mediante una estructura de memoria **AoS** (*Array of Structures*).

---

## Estructura del Proyecto

```text
.
├── .devcontainer/          # Entorno de desarrollo aislado con Docker
│   ├── Dockerfile          # Definición del contenedor de compilación
│   └── devcontainer.json   # Configuración para VS Code
├── archivos_config/        # Archivos de configuración de prueba (.txt) e imágenes (.ppm)
├── aos/                    # Código fuente del módulo AoS
│   ├── include/
│   │   └── image_aos.hpp   # Definición e interfaz del renderizador
│   └── src/
│       ├── image_aos.cpp   # Implementación del procesamiento de imágenes
│       └── main.cpp        # Punto de entrada de la aplicación
├── .clang-format           # Reglas de estilo de código
├── .clang-tidy             # Configuración de análisis estático
├── CMakeLists.txt          # Guión principal de compilación CMake
└── CMakePresets.json       # Ajustes predeterminados para CMake

```

---

## Requisitos Previos

* **Compilador de C++** compatible con C++20 (GCC 10+, Clang 11+ o MSVC).
* **CMake** (versión 3.15 o superior).


* **Ninja** o **Make** como generador de compilación.
* *(Opcional)* **Docker** y **VS Code** con la extensión *Dev Containers*.



---

## Compilación y Ejecución

### Opción 1: Uso de DevContainer (Recomendado)

1. Abrir el repositorio en **Visual Studio Code**.
2. Seleccionar la opción **Reopen in Container** cuando el sistema lo solicite (o ejecutar la orden mediante `Ctrl+Shift+P` -> `Dev Containers: Reopen in Container`).
3. El entorno configurará automáticamente los compiladores, CMake y las herramientas de análisis de código.



### Opción 2: Compilación manual con CMake

1. Generar los archivos de construcción:
```bash
cmake -B build -S .

```


2. Compilar el ejecutable:
```bash
cmake --build build

```


3. Ejecutar la aplicación pasando un archivo de configuración como argumento:
```bash
./build/aos/aos_render archivos_config/config1.txt

```



---

## Calidad y Formato de Código

El proyecto incluye archivos de configuración para mantener un estándar de calidad homogéneo mediante `clang-format` y `clang-tidy`.

* **Aplicar formato automático:**
```bash
clang-format -i aos/src/*.cpp aos/include/*.hpp

```


* **Ejecutar el análisis estático:**
```bash
clang-tidy aos/src/*.cpp -- -Iaos/include

```
