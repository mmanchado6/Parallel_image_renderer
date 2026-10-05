#include "../include/scene.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "material.hpp"
#include "object.hpp"

// Añadir elementos a la escena
void render::scene::add_material(material::material const & material) {
  // Verificar que el material no existe ya en la escena para poder ser añadido
  auto result = materials_.emplace(material.name(), material);
  if (!result.second) {
    throw std::invalid_argument("Material with name '" + material.name() + "' already exists.");
  }
}

void render::scene::add_sphere(object::sphere const & sphere) {
  // Verificar que el material de la esfera existe para poder ser añadida
  if (!materials_.contains(sphere.materialName())) {
    throw std::invalid_argument(
        "Material with name '" + sphere.materialName() + "' does not exist.");
  }
  spheres_.push_back(sphere);
}

void render::scene::add_cylinder(object::cylinder const & cylinder) {
  // Verificar que el material del cilindro existe para poder ser añadido
  if (!materials_.contains(cylinder.materialName())) {
    throw std::invalid_argument(
        "Material with name '" + cylinder.materialName() + "' does not exist.");
  }
  cylinders_.push_back(cylinder);
}

// Obtener elementos de la escena

material::material const * render::scene::material(std::string const & name) const {
  // Buscar el material por su nombre
  auto it = materials_.find(name);
  if (it != materials_.end()) {
    return &(it->second);
  }
  return nullptr;  // Material no encontrado
}

std::vector<object::sphere> const & render::scene::spheres() const {
  // Devolver la lista de esferas
  return spheres_;
}

std::vector<object::cylinder> const & render::scene::cylinders() const {
  // Devolver la lista de cilindros
  return cylinders_;
}

std::unordered_map<std::string, material::material> const & render::scene::all_materials() const {
  // Devolver el mapa de materiales
  return materials_;
}

// Consultas útiles
bool render::scene::empty() const {
  // Verificar si la escena está vacía
  return spheres_.empty() and cylinders_.empty();
}

size_t render::scene::total_objects() const {
  // Devolver el número total de objetos en la escena
  return spheres_.size() + cylinders_.size();
}
