#ifndef INTERSECCION_HPP
#define INTERSECCION_HPP

#include "object.hpp"
#include "ray.hpp"
#include "vector.hpp"

namespace render {

  struct interseccion_params {
    // Estructura para pasar parámetros al constructor de intersección
    render::point punto{0, 0, 0};
    render::vector normal{0, 0, 0};
    double distancia{};
    bool cara_frontal{};
    ray::ray rayo{
      render::point{  0,   0,   0},
      render::vector{0.0, 0.0, 0.0}
    };
    object::object const * object{nullptr};
    material::material const * material{nullptr};
  };

  class interseccion {
    // Clase que representa la intersección entre un rayo y un objeto
  private:
    render::point pt;
    render::vector nm;
    double dist;
    bool c_f;
    ray::ray ryo;

    // Puntero para evitar la comprobación del tipo de objeto
    object::object const * obj;
    material::material const * mat;

  public:
    // Constructor
    interseccion(interseccion_params const & params)
        : pt{params.punto}, nm{params.normal}, dist{params.distancia}, c_f{params.cara_frontal},
          ryo{params.rayo}, obj{params.object}, mat{params.material} { }

    // Getters
    [[nodiscard]] render::point posicion() const { return pt; }

    [[nodiscard]] render::vector normal() const { return nm; }

    [[nodiscard]] double distancia() const { return dist; }

    [[nodiscard]] bool cara_frontal() const { return c_f; }

    [[nodiscard]] ray::ray rayo() const { return ryo; }

    [[nodiscard]] object::object const * object() const { return obj; }

    [[nodiscard]] material::material const * material() const { return mat; }
  };

}  // namespace render

#endif
