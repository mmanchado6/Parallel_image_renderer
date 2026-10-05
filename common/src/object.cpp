#include "../include/object.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <algorithm>
#include <cmath>

namespace object {

  // Funciones para calculo de interseccion
  double object::calculate_lambda(double a, double b, double c) {
    // Función auxiliar para calcular lambda en intersecciones
    // Verificar caso degenerado
    if (std::abs(a) < 1e-12) {
      return -1.0;
    }

    double discriminante = b * b - 4 * a * c;

    // Usar tolerancia para evitar problemas de precisión
    if (discriminante < -1e-12) {
      return -1.0;
    }

    // Asegurar que discriminante no sea negativo
    discriminante = std::max(0.0, discriminante);

    double const sqrt_d  = std::sqrt(discriminante);
    double const lambda1 = (-b - sqrt_d) / (2.0 * a);
    double const lambda2 = (-b + sqrt_d) / (2.0 * a);

    // Encontrar la intersección válida más cercana
    double lambda = -1.0;

    if (lambda1 >= 0.001) {
      lambda = lambda1;
    }
    if (lambda2 >= 0.001 and (lambda < 0 or lambda2 < lambda)) {
      lambda = lambda2;
    }

    return lambda;
  }

  object::paramsABC sphere::compute_params(ray::ray const & rayo) const {
    // Calcular los parámetros a, b, c para la ecuación cuadrática de intersección
    render::vector const r_c = render::vector::get_vector_from_points(position(), rayo.origin());
    render::vector const d_r = rayo.direction();

    // Coeficientes de la ecuación cuadrática
    double const a = d_r.dot(d_r);                          // a = d_r · d_r
    double const b = 2.0 * d_r.dot(r_c);                    // b = 2 * (d_r · r_c)
    double const c = r_c.dot(r_c) - (radius() * radius());  // c = r_c · r_c - r^2

    return object::paramsABC() = {a, b, c};
  }

  object::paramsABC cylinder::compute_params(ray::ray const & rayo) const {
    // Calcular los parámetros a, b, c para la ecuación cuadrática de intersección
    render::vector const r_c   = render::vector::get_vector_from_points(position(), rayo.origin());
    render::vector const d_r   = rayo.direction();
    render::vector const a_hat = axis().normalize();  // eje unitario
    double const radio         = radius();

    render::vector const r_c_perp = r_c - a_hat * (r_c.dot(a_hat));
    render::vector const d_r_perp = d_r - a_hat * (d_r.dot(a_hat));

    double const a = d_r_perp.dot(d_r_perp);                    // a = d_r_perp · d_r_perp
    double const b = 2.0 * r_c_perp.dot(d_r_perp);              // b = 2 * (r_c_perp · d_r_perp)
    double const c = r_c_perp.dot(r_c_perp) - (radio * radio);  // c = r_c_perp · r_c_perp - r^2

    return object::paramsABC() = {a, b, c};
  }

  object::paramsABC cylinder::compute_params_base(ray::ray const & rayo,
                                                  render::point const & centro_base,
                                                  render::vector const & normal_base) {
    // Calcular los parámetros para la intersección con la base del cilindro
    render::vector const d_r = rayo.direction();
    render::vector const r_p = render::vector::get_vector_from_points(rayo.origin(), centro_base);
    double const denominador = d_r.dot(normal_base);
    double const lambda      = r_p.dot(normal_base) / denominador;
    double const distancia_al_centro =
        render::vector::get_vector_from_points(centro_base, rayo.at(lambda)).magnitude();

    return object::paramsABC() = {denominador, lambda, distancia_al_centro};
  }

  bool cylinder::is_in_cylinder_limits(render::vector a_hat, double lambda,
                                       ray::ray const & rayo) const {
    // Comprobar si la intersección está dentro de los límites del cilindro
    render::point const punto_interseccion = rayo.at(lambda);

    render::vector const vector_desde_centro =
        render::vector::get_vector_from_points(position(), punto_interseccion);
    double const proyeccion_altura = vector_desde_centro.dot(a_hat);
    return std::abs(proyeccion_altura) <= height() / 2.0;
  }

}  // namespace object
