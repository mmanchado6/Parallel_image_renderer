#ifndef RAY_HPP
#define RAY_HPP

#include "vector.hpp"

namespace ray {

  // TODO: Sacar de aquí el cálculo de las direcciónes (getVectorFromPoints(origin, Q)) para evitar
  // así tener q hacer otra clase ray con los problemas en intersección que esto conlleva
  class ray {
    // Clase que representa un rayo en el espacio 3D
  public:
    ray(render::point const & origin, render::vector const & direction)
        : origin_(origin), direction_(direction) { }

    [[nodiscard]] render::point at(double lambda) const { return origin_ + (direction_ * lambda); }

    [[nodiscard]] render::point const & origin() const { return origin_; }

    [[nodiscard]] render::vector direction() const { return direction_; }

  private:
    render::point origin_;
    render::vector direction_;
  };

}  // namespace ray

#endif
