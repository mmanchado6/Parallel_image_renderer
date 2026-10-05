#!/usr/bin/env python3

import math
import sys


class Pixel:
    def __init__(self, r=0, g=0, b=0):
        self.r = r
        self.g = g
        self.b = b


def leer_ppm(filename):
    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
    except IOError:
        print(f"Error al abrir el archivo: {filename}")
        return False, None, None, None

    # Procesar líneas ignorando comentarios
    processed_lines = []
    for line in lines:
        stripped_line = line.strip()
        if stripped_line and not stripped_line.startswith('#'):
            processed_lines.append(stripped_line)

    if not processed_lines or processed_lines[0] != "P3":
        print(f"Formato no soportado (debe ser P3): {processed_lines[0] if processed_lines else 'empty file'}")
        return False, None, None, None

    # Leer dimensiones
    try:
        dimensions = processed_lines[1].split()
        width = int(dimensions[0])
        height = int(dimensions[1])
        max_val = int(processed_lines[2])
    except (IndexError, ValueError) as e:
        print(f"Error leyendo dimensiones o valor máximo: {e}")
        return False, None, None, None

    # Leer datos de píxeles
    data = []
    pixel_values = []

    # Recoger todos los valores numéricos
    for i in range(3, len(processed_lines)):
        values = processed_lines[i].split()
        pixel_values.extend(map(int, values))

    # Crear objetos Pixel
    if len(pixel_values) < width * height * 3:
        print(
            f"Error: No hay suficientes datos de píxeles. Esperados: {width * height * 3}, Obtenidos: {len(pixel_values)}")
        return False, None, None, None

    for i in range(0, width * height * 3, 3):
        pixel = Pixel(pixel_values[i], pixel_values[i + 1], pixel_values[i + 2])
        data.append(pixel)

    return True, width, height, data


def diferencia_pixel(a, b):
    return (abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b)) / 3.0


def main():
    if len(sys.argv) != 3:
        print(f"Uso: {sys.argv[0]} imagen1.ppm imagen2.ppm")
        return 1

    file1 = sys.argv[1]
    file2 = sys.argv[2]

    # Leer primera imagen
    success1, width1, height1, img1 = leer_ppm(file1)
    if not success1:
        return 1

    # Leer segunda imagen
    success2, width2, height2, img2 = leer_ppm(file2)
    if not success2:
        return 1

    # Verificar dimensiones
    if width1 != width2 or height1 != height2:
        print("Las imágenes tienen diferente tamaño")
        return 1

    if len(img1) != len(img2):
        print("Error: Las imágenes tienen diferente número de píxeles")
        return 1

    # Calcular diferencias
    max_diff = 0.0
    sum_squared_diff = 0.0

    for i in range(len(img1)):
        diff = diferencia_pixel(img1[i], img2[i])
        if diff > max_diff:
            max_diff = diff
        sum_squared_diff += diff * diff

    mse = math.sqrt(sum_squared_diff / len(img1))

    print(f"Diferencia máxima: {max_diff}")
    print(f"Error cuadrático medio (MSE): {mse}")

    if max_diff < 150 and mse < 10:
        print("Resultado: aceptable")
    else:
        print("Resultado: NO aceptable")

    return 0


if __name__ == "__main__":
    sys.exit(main())