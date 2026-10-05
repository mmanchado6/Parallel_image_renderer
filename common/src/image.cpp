#include "../include/image.hpp"
#include "../include/vector.hpp"
#include <cmath>
#include <random>

namespace imagec {

  std::mt19937_64 imageConfig::material_gen(13);

  void imageConfig::init_material_rng(std::mt19937_64::result_type seed) {
    material_gen.seed(seed);
  }

  render::vector Pov::dirv_u() const {
    render::vector const vhat = focal_vector().normalize();
    render::vector u          = n.cross(vhat);
    double unorm              = u.magnitude();

    if (unorm < 1e-8 or std::isnan(unorm)) {
      render::vector const arbitrary =
          (std::fabs(vhat.X()) < 0.9) ? render::vector(1, 0, 0) : render::vector(0, 1, 0);
      u     = arbitrary.cross(vhat);
      unorm = u.magnitude();

      // Si aún así es cero, usar un vector arbitrario
      if (unorm < 1e-8 or std::isnan(unorm)) {
        return {1, 0, 0};
      }
    }

    return u * (1.0 / unorm);
  }

}  // namespace imagec
