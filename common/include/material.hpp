#ifndef material_HPP
#define material_HPP

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace material {

  class material {
    // Clase material que contiene el nombre, tipo y parámetros del material
  public:
    // Posibles tipos de material
    enum class materialType { Matte, Metal, Refractive };

    material(std::string name, materialType type, std::vector<double> params)
        : nm{std::move(name)}, tp{type}, pms{std::move(params)} {
      setType(type);
    }

    // Getters
    [[nodiscard]] std::string name() const { return nm; }

    [[nodiscard]] materialType type() const { return tp; }

    [[nodiscard]] std::vector<double> params() const { return pms; }

    // Setter
    // TODO: Manejar los errores
    void setType(materialType t) {
      switch (t) {
        case materialType::Matte:
          if (pms.size() == 3) {
            tp = t;
          } else {
            throw std::invalid_argument("Error: Matte requiere 3 parámetros");
          }
          break;

        case materialType::Metal:
          if (pms.size() == 4) {
            tp = t;
          } else {
            throw std::invalid_argument("Error: Metal requiere 4 parámetros");
          }
          break;

        case materialType::Refractive:
          if (pms.size() == 1) {
            tp = t;
          } else {
            throw std::invalid_argument("Error: Refractive requiere 1 parámetro");
          }
          break;
      }
    }

    void setPms(std::vector<double> p) { pms = std::move(p); }

  private:
    std::string nm;
    materialType tp;
    std::vector<double> pms;
  };

};  // namespace material

// namespace material

#endif
