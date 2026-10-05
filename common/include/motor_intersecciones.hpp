#include "image.hpp"
#include "interseccion.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "vector.hpp"
#include <optional>

namespace render {

  class motorIntersecciones {
  public:
    struct cylinder_data {
      object::cylinder const * cilindro{};
      ray::ray const * rayo{};
      object::cylinder::paramsABC parametros{};
      std::optional<interseccion> interseccion_cercana;
      double distancia_cercana{};
      int direction{};
    };

    // Encontrar la intersección más cercana
    static std::optional<interseccion> cylinder_intersection_comparison(
        ray::ray const & rayo, object::cylinder const & cilindro);

    static std::pair<std::optional<interseccion>, double> lateral_cylinder_intersection_comparison(
        cylinder_data data);

    static std::pair<std::optional<interseccion>, double> base_cylinder_intersection_comparison(
        cylinder_data const & data);

    static std::optional<render::interseccion> find_nearest_intersection(ray::ray const & rayo,
                                                                         scene const & escena);

    static render::interseccion sphere_intersection(ray::ray const & rayo,
                                                    object::object const & objeto, double lambda);

    static render::interseccion lateral_cylinder_intersection(ray::ray const & rayo,
                                                              object::cylinder const & cilindro,
                                                              double lambda, render::vector a_hat);

    static render::interseccion base_cylinder_intersection(ray::ray const & rayo,
                                                           object::cylinder const & cilindro,
                                                           double lambda,
                                                           render::vector const & normal_base);

    static double random_num(double min, double max) {
      std::uniform_real_distribution<double> dist(min, max);
      return dist(imagec::imageConfig::get_material_gen());
    }

    static vector matte_reflection_ray_vector(interseccion const & intersection);

    static vector metal_reflection_ray_vector(interseccion const & intersection);

    static vector refractive_reflection_ray_vector(interseccion const & intersection);

    // Contribución al color

    static render::vector reflected_dir(interseccion const & interseccion);

    static ray::ray build_bounced_ray(interseccion const & interseccion,
                                      render::vector const & dir) {
      ray::ray bounced_ray(interseccion.posicion(), dir);
      return bounced_ray;
    }

    static imagec::Color trace_child_ray(ray::ray const & bounced, int child_depth,
                                         scene const & scene, imagec::imageConfig const & confg);
    static void material_reflectance(interseccion const & interseccion, double & rx, double & ry,
                                     double & rz);

    static imagec::Color color_contribution(interseccion const & interseccion, int depth,
                                            scene const & scene, imagec::imageConfig const & confg);

    static imagec::Color mul(imagec::Color const & a, double rx, double ry, double rz) {
      // Función auxiliar para multiplicar una tupla rgb con 3 valores rgb
      auto [ax, ay, az] = a;
      return {ax * rx, ay * ry, az * rz};
    }

    static imagec::Color mul(imagec::Color const & a, imagec::Color const & b) {
      // Función auxiliar para multiplicar tuplas de rgb
      auto [ax, ay, az] = a;
      auto [bx, by, bz] = b;
      return {ax * bx, ay * by, az * bz};
    }

    static imagec::Color background_color(ray::ray const & r, imagec::imageConfig const & conf);
  };

}  // namespace render
