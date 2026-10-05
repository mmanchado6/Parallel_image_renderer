#ifndef RENDER_SCENE_HPP
#define RENDER_SCENE_HPP
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "material.hpp"
#include "object.hpp"

namespace render {

  class scene {
    // Clase escena que contiene todos los objetos y materiales
  private:
    std::vector<object::sphere> spheres_;
    std::vector<object::cylinder> cylinders_;
    std::unordered_map<std::string, material::material> materials_;

  public:
    scene() = default;

    // Añadir elementos a la escena
    void add_material(material::material const & material);
    void add_sphere(object::sphere const & sphere);
    void add_cylinder(object::cylinder const & cylinder);

    // Obtener elementos de la escena
    material::material const * material(std::string const & name) const;
    std::vector<object::sphere> const & spheres() const;
    std::vector<object::cylinder> const & cylinders() const;
    std::unordered_map<std::string, material::material> const & all_materials() const;

    // Consultas útiles
    bool empty() const;
    size_t total_objects() const;
  };

}  // namespace render

#endif  // RENDER_SCENE_HPP
