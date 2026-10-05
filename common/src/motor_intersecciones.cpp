#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

#include "../include/image.hpp"
#include "../include/motor_intersecciones.hpp"
#include "../include/object.hpp"
#include "../include/ray.hpp"
#include <tuple>
#include <utility>

#include "image.hpp"
#include "interseccion.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "vector.hpp"

namespace render {

  // Funciones para encontrar la intersección más cercana

  std::optional<interseccion> motorIntersecciones::cylinder_intersection_comparison(
      // Comparar intersecciones con las distintas partes del cilindro
      ray::ray const & rayo, object::cylinder const & cilindro) {
    std::optional<interseccion> interseccion_cercana;
    double distancia_cercana = std::numeric_limits<double>::max();

    // Intersección con superficie lateral
    auto lateral_params = cilindro.compute_params(rayo);
    cylinder_data const curved_data_1{
      &cilindro, &rayo, lateral_params, interseccion_cercana, distancia_cercana, 0};
    std::tie(interseccion_cercana, distancia_cercana) =
        lateral_cylinder_intersection_comparison(curved_data_1);

    // Intersección con base superior
    cylinder_data const base_superior_data{
      &cilindro, &rayo, lateral_params, interseccion_cercana, distancia_cercana, 1};
    std::tie(interseccion_cercana, distancia_cercana) =
        base_cylinder_intersection_comparison(base_superior_data);

    // Intersección con base inferior
    cylinder_data const base_inferior_data{
      &cilindro, &rayo, lateral_params, interseccion_cercana, distancia_cercana, -1};
    std::tie(interseccion_cercana, distancia_cercana) =
        base_cylinder_intersection_comparison(base_inferior_data);

    // Resultado final
    if (interseccion_cercana and interseccion_cercana->distancia() < 0) {
      return std::nullopt;  // No devolver intersecciones inválidas
    }

    return interseccion_cercana;
  }

  std::pair<std::optional<interseccion>, double>
  motorIntersecciones::lateral_cylinder_intersection_comparison(cylinder_data data) {
    // Función para comparar la intersección lateral del cilindro
    double const lambda =
        object::cylinder::calculate_lambda(data.parametros.a, data.parametros.b, data.parametros.c);
    // Variables locales en lugar de modificar data
    auto interseccion_cercana = data.interseccion_cercana;
    auto distancia_cercana    = data.distancia_cercana;

    // Solo proceder si lambda es válido
    if (lambda >= 0.001) {
      render::vector const eje_unitario = data.cilindro->axis().normalize();
      if (data.cilindro->is_in_cylinder_limits(eje_unitario, lambda, *data.rayo)) {
        auto interseccion_curva = render::motorIntersecciones::lateral_cylinder_intersection(
            *data.rayo, *data.cilindro, lambda, eje_unitario);
        double const interseccion_superior_distancia = interseccion_curva.distancia();
        if (interseccion_superior_distancia < data.distancia_cercana) {
          interseccion_cercana = interseccion_curva;
          distancia_cercana    = interseccion_superior_distancia;
        }
      }
    }
    // Si lambda no es válido se devuelven los valores actuales
    return {interseccion_cercana, distancia_cercana};
  }

  std::pair<std::optional<interseccion>, double>
  motorIntersecciones::base_cylinder_intersection_comparison(cylinder_data const & data) {
    // Función para comparar la intersección con las bases del cilindro
    double const altura               = data.cilindro->height();
    render::vector const eje_unitario = data.cilindro->axis().normalize();
    render::point const centro =
        data.cilindro->position() + (eje_unitario * (altura / 2.0) * data.direction);
    render::vector const normal = eje_unitario * data.direction;
    object::cylinder::paramsABC const parametros_base_superior =
        object::cylinder::compute_params_base(*data.rayo, centro, normal);

    double const denominador         = parametros_base_superior.a;
    double const lambda              = parametros_base_superior.b;
    double const distancia_al_centro = parametros_base_superior.c;

    // Variables locales en lugar de modificar data
    auto interseccion_cercana = data.interseccion_cercana;
    auto distancia_cercana    = data.distancia_cercana;

    // Solo proceder si lambda es válido
    if (lambda >= 0.001 and
        std::abs(denominador) >= 1e-8 and
        distancia_al_centro <= data.cilindro->radius())
    {
      auto interseccion_superior = render::motorIntersecciones::base_cylinder_intersection(
          *data.rayo, *data.cilindro, lambda, normal);
      double const interseccion_superior_distancia = interseccion_superior.distancia();
      if (interseccion_superior_distancia < distancia_cercana) {
        interseccion_cercana = interseccion_superior;
        distancia_cercana    = interseccion_superior_distancia;
      }
    }
    return {interseccion_cercana, distancia_cercana};
  }

  render::interseccion motorIntersecciones::sphere_intersection(ray::ray const & rayo,
                                                                object::object const & objeto,
                                                                double lambda) {
    // Intersección con esfera
    // Calculamos el punto de intersección
    render::point const punto_interseccion = rayo.at(lambda);

    // Calculamos la normal en el punto de intersección
    render::vector const normal_interseccion =
        (render::vector::get_vector_from_points(objeto.position(), punto_interseccion)) /
        objeto.radius();
    vector const normal_interseccion1 = (rayo.direction().dot(normal_interseccion) < 0)
                                            ? normal_interseccion
                                            : -normal_interseccion;

    render::interseccion_params const params{punto_interseccion,
                                             normal_interseccion1,
                                             lambda,
                                             rayo.direction().dot(normal_interseccion) < 0,
                                             rayo,
                                             &objeto,
                                             &objeto.material()};

    return render::interseccion{params};
  }

  render::interseccion motorIntersecciones::lateral_cylinder_intersection(
      ray::ray const & rayo, object::cylinder const & cilindro, double lambda,
      render::vector a_hat) {
    // Intersección con superficie lateral del cilindro
    render::point const punto_interseccion = rayo.at(lambda);
    render::vector const vector_desde_centro =
        render::vector::get_vector_from_points(cilindro.position(), punto_interseccion);
    double const proyeccion_altura = vector_desde_centro.dot(a_hat);

    // Calculamos la normal en el punto de intersección
    render::vector const normal = vector_desde_centro - (a_hat * proyeccion_altura);

    vector const normal_interseccion1 = (rayo.direction().dot(normal) < 0) ? normal : -normal;

    interseccion_params const params{
      punto_interseccion, normal_interseccion1, lambda, rayo.direction().dot(normal) < 0, rayo,
      &cilindro,          &cilindro.material()};

    return interseccion{params};
  }

  render::interseccion motorIntersecciones::base_cylinder_intersection(
      // Intersección con base del cilindro
      ray::ray const & rayo, object::cylinder const & cilindro, double lambda,
      render::vector const & normal_base) {
    render::point const punto_interseccion = rayo.at(lambda);
    double const cara_frontal_temp         = rayo.direction().dot(normal_base);

    vector const normal_interseccion1 = (cara_frontal_temp < 0) ? normal_base : -normal_base;

    interseccion_params const params{
      punto_interseccion, normal_interseccion1, lambda, cara_frontal_temp < 0, rayo,
      &cilindro,          &cilindro.material()};

    return interseccion{params};
  }

  std::optional<interseccion> motorIntersecciones::find_nearest_intersection(ray::ray const & rayo,
                                                                             scene const & escena) {
    std::optional<interseccion> closest;
    double min_distance = 1e10;
    for (auto const & esfera : escena.spheres()) {
      render::vector const to_sphere =
          render::vector::get_vector_from_points(rayo.origin(), esfera.position());
      if (rayo.direction().dot(to_sphere) < 0) {
        continue;  // Si el rayo no va a intersectar con la esfera pasamos a la siguiente
      }

      auto params         = esfera.compute_params(rayo);
      double const lambda = object::sphere::calculate_lambda(params.a, params.b, params.c);

      if (lambda >= 0.001 and lambda < min_distance) {
        auto intersection = sphere_intersection(rayo, esfera, lambda);
        min_distance      = intersection.distancia();
        closest           = intersection;
      }
    }

    // Para cilindros
    for (auto const & cilindro : escena.cylinders()) {
      auto intersection = cylinder_intersection_comparison(rayo, cilindro);
      if (intersection and intersection->distancia() < min_distance) {
        min_distance = intersection->distancia();
        closest      = intersection;
      }
    }

    return closest;
  }

  vector motorIntersecciones::matte_reflection_ray_vector(interseccion const & intersection) {
    // Dirección de reflexión matte
    // Generar tres valores aleatorios independientes para cada componente
    double const rx = motorIntersecciones::random_num(-1, 1);
    double const ry = motorIntersecciones::random_num(-1, 1);
    double const rz = motorIntersecciones::random_num(-1, 1);

    vector const random_vec(rx, ry, rz);
    vector const intersection_mod = intersection.normal() + random_vec;

    // Verificación
    if (std::abs(intersection_mod.X()) < 1e-8 and
        std::abs(intersection_mod.Y()) < 1e-8 and
        std::abs(intersection_mod.Z()) < 1e-8)
    {
      return intersection.normal();
    }

    return intersection_mod.normalize();
  }

  vector motorIntersecciones::metal_reflection_ray_vector(interseccion const & intersection) {
    // Direccion de reflexion metálica
    double const prod_origin_normal = intersection.rayo().direction().dot(intersection.normal());
    double const factor             = 2 * prod_origin_normal;

    vector const reflex_1 =
        (intersection.rayo().direction() - (intersection.normal() * factor)).normalize();

    double const difusion = intersection.object()->material().params()[3];

    vector const random_vec(motorIntersecciones::random_num(-difusion, difusion),
                            motorIntersecciones::random_num(-difusion, difusion),
                            motorIntersecciones::random_num(-difusion, difusion));
    return (reflex_1 + random_vec).normalize();
  }

  vector motorIntersecciones::refractive_reflection_ray_vector(interseccion const & intersection) {
    // Dirección de refracción refractiva
    vector const u_rvector   = intersection.rayo().direction().normalize();
    vector const nvector     = intersection.normal();
    double const scalar_prod = -u_rvector.dot(nvector);

    double const cos_refr = std::min(scalar_prod, 1.0);
    double const sin_sq   = std::sqrt(1 - (cos_refr * cos_refr));

    double const refr_index = intersection.object()->material().params()[0];
    double const refr_index_corregido =
        intersection.cara_frontal() ? (1.0 / refr_index) : refr_index;

    if (refr_index_corregido * sin_sq > 1.0) {
      return u_rvector - (nvector * (2.0 * -scalar_prod));
    }

    vector const u     = ((nvector * cos_refr) + u_rvector) * refr_index_corregido;
    double const u_mod = u.magnitude();
    return u + nvector * (-std::sqrt(std::abs(1.0 - u_mod * u_mod)));
  }

  render::vector motorIntersecciones::reflected_dir(interseccion const & interseccion) {
    // Dirección de rebote según material
    auto const & mat = interseccion.object()->material();

    switch (mat.type()) {
      case material::material::materialType::Matte:
        return motorIntersecciones::matte_reflection_ray_vector(interseccion);
      case material::material::materialType::Refractive:
        return motorIntersecciones::refractive_reflection_ray_vector(interseccion);
      case material::material::materialType::Metal:
        return motorIntersecciones::metal_reflection_ray_vector(interseccion);
      default: return {0.0, 0.0, 0.0};  // Negro para error
    }
  }

  //  Trazo del hijo (función recursiva)
  imagec::Color motorIntersecciones::trace_child_ray(ray::ray const & bounced, int child_depth,
                                                     scene const & scene,
                                                     imagec::imageConfig const & confg) {
    // Trazado de los rayos hijo
    if (auto next_interseccion = motorIntersecciones::find_nearest_intersection(bounced, scene)) {
      return color_contribution(*next_interseccion, child_depth, scene, confg);
    }
    // No hay intersección -> color fondo
    return background_color(bounced, confg);
  }

  imagec::Color motorIntersecciones::color_contribution(interseccion const & interseccion,
                                                        int depth, scene const & scene,
                                                        imagec::imageConfig const & confg) {
    // Contribución del color
    if (depth <= 0) {
      return {0.0, 0.0, 0.0};
    }
    auto const * mat = interseccion.material();

    render::vector const ray_new_dir = reflected_dir(interseccion);
    ray::ray const bounced(interseccion.posicion(), ray_new_dir);

    // Llamada recursiva
    imagec::Color child = trace_child_ray(bounced, depth - 1, scene, confg);

    // Ponemos 1 para cuando es refractivo
    auto [cr, cg, cb] = child;
    double rx = 1.0, ry = 1.0, rz = 1.0;

    if (mat->type() != material::material::materialType::Refractive) {
      rx = mat->params()[0];
      ry = mat->params()[1];
      rz = mat->params()[2];
    }
    return {cr * rx, cg * ry, cb * rz};
  }

  imagec::Color motorIntersecciones::background_color(ray::ray const & r,
                                                      imagec::imageConfig const & conf) {
    double const m  = (r.direction().normalize().Y() + 1.0) * 0.5;
    double const w0 = 1.0 - m;

    auto const & cd = conf.background_dark();
    auto const & cl = conf.background_light();

    auto [cdx, cdy, cdz] = cd;
    auto [clx, cly, clz] = cl;

    return {w0 * clx + m * cdx, w0 * cly + m * cdy, w0 * clz + m * cdz};
  }

}  // namespace render
