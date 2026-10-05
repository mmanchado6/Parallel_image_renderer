#include "../include/scene_parser.hpp"
#include "../include/material.hpp"
#include "../include/object.hpp"
#include "../include/parser.hpp"
#include "../include/vector.hpp"
#include "scene.hpp"
#include <cstddef>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

render::scene sceneParser::parse(std::string const & filename) {
  // Función utilizada para parsear el archivo de entrada y rellenar una clase Scene
  render::scene scene;
  std::ifstream file(filename);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  std::string line;
  int line_number = 0;
  // Procesar línea por línea
  while (std::getline(file, line)) {
    line_number++;
    try {
      process_line(line, scene);
    } catch (std::exception const & e) {
      throw std::runtime_error("Error in line " + std::to_string(line_number) + ": " + e.what());
    }
  }

  file.close();
  return scene;
}

void sceneParser::process_line(std::string const & line, render::scene & scene) {
  // Función para procesar una línea individual del archivo
  // Ignorar las líneas vacías
  if (parser::is_blank_line(line)) {
    return;
  }

  // Dividir la línea en tokens

  auto tokens = parser::split_line(line);

  if (tokens.empty()) {
    return;
  }

  // Identificar el tipo de dato por el primer token

  std::string const & type = tokens[0];

  if (type == "matte:" or type == "metal:" or type == "refractive:") {
    parse_material(line, scene);

  } else if (type == "sphere:") {
    parse_sphere(line, scene);

  } else if (type == "cylinder:") {
    parse_cylinder(line, scene);

  } else {
    throw std::invalid_argument("Unknown element type: " + type);
  }
}

// Funciones para la lectura de cada tipo posible de dato

void sceneParser::parse_material(std::string const & line, render::scene & scene) {
  auto tokens = parser::split_line(line);
  // Funcion para parsear materiales
  // Validar número mínimo de tokens
  if (tokens.size() < 3) {
    throw std::invalid_argument("Insufficient parameters for material.");
  }

  // Leer parámetros
  std::string const & name   = tokens[1];
  std::vector<double> params = {};

  for (size_t i = 2; i < tokens.size(); ++i) {
    params.push_back(std::stod(tokens[i]));
  }

  // Crear material según el tipo
  if (tokens[0] == "matte:") {
    std::vector<double> const float_params(params.begin(), params.end());
    material::material const material(name, material::material::materialType::Matte, float_params);
    scene.add_material(material);
  } else if (tokens[0] == "metal:") {
    std::vector<double> const float_params(params.begin(), params.end());
    material::material const material(name, material::material::materialType::Metal, float_params);
    scene.add_material(material);
  } else if (tokens[0] == "refractive:") {
    std::vector<double> const float_params(params.begin(), params.end());
    material::material const material(name, material::material::materialType::Refractive,
                                      float_params);
    scene.add_material(material);
  } else {
    throw std::invalid_argument("Unknown material type: " + tokens[0]);
  }
}

void sceneParser::parse_sphere(std::string const & line, render::scene & scene) {
  auto tokens = parser::split_line(line);

  // Función para parsear esferas

  // Validar número mínimo de tokens

  if (tokens.size() != 6) {
    throw std::invalid_argument("Insufficient parameters for sphere.");
  }

  // Leer parámetros

  double const x = std::stod(tokens[1]);

  double const y = std::stod(tokens[2]);

  double const z = std::stod(tokens[3]);

  render::point const point(x, y, z);

  double const radius = std::stod(tokens[4]);

  std::string const & material_name = tokens[5];

  material::material const * material = scene.material(material_name);

  object::sphere const sphere(point, radius, *material);

  scene.add_sphere(sphere);
}

void sceneParser::parse_cylinder(std::string const & line, render::scene & scene) {
  auto tokens = parser::split_line(line);

  // Función para parsear cilindros

  // Validar número mínimo de tokens

  if (tokens.size() != 9) {
    throw std::invalid_argument("Insufficient parameters for cylinder.");
  }

  // Leer parámetros

  double const x = std::stod(tokens[1]);

  double const y = std::stod(tokens[2]);

  double const z = std::stod(tokens[3]);

  render::point const point(x, y, z);

  double const radius = std::stod(tokens[4]);

  double const ax = std::stod(tokens[5]);

  double const ay = std::stod(tokens[6]);

  double const az = std::stod(tokens[7]);

  render::vector const axis(ax, ay, az);

  std::string const & material_name = tokens[8];

  material::material const * material = scene.material(material_name);

  object::cylinder const cylinder(point, radius, axis, *material);

  scene.add_cylinder(cylinder);
}
