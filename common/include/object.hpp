#ifndef object_HPP
#define object_HPP

#include <utility>

#include "material.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <cmath>

namespace object {

  class object {
    // Clase base abstracta para objetos 3D
  public:
    // Evitar copias innecesarias
    object(render::point const & pos, double r, material::material mat)
        : position_{pos}, radius_{r}, material_{std::move(mat)} { }

    virtual ~object() = default;  // Destructor virtual

    // Operaciones de copia y movimiento
    object(object const &) = default;

    object & operator=(object const &)     = default;
    object(object &&) noexcept             = default;
    object & operator=(object &&) noexcept = default;

    struct paramsABC {
      double a;
      double b;
      double c;
    };

    // Getters
    [[nodiscard]] render::point position() const { return position_; }

    [[nodiscard]] double radius() const { return radius_; }

    [[nodiscard]] material::material const & material() const { return material_; }

    [[nodiscard]] std::string materialName() const { return material_.name(); }

    [[nodiscard]] virtual render::vector normal(render::point const & punto) const = 0;

    // Funciones para calculo de interseccion
    [[nodiscard]] virtual object::paramsABC compute_params(ray::ray const & rayo) const = 0;

    static double calculate_lambda(double a, double b, double c);

  private:
    render::point position_;
    double radius_;
    material::material material_;
  };

  class sphere : public object {
    // Clase derivada para esferas
  public:
    sphere(render::point const & pos, double r, material::material const & mat)
        : object(pos, r, mat) { }

    // Getters
    [[nodiscard]] render::vector normal(render::point const & ipoint) const override {
      render::vector normal = render::vector::get_vector_from_points(position(), ipoint);
      normal                = normal / radius();
      return normal.normalize();
    }

    // Funciones para calculo de interseccion
    [[nodiscard]] object::paramsABC compute_params(ray::ray const & rayo) const override;
  };

  class cylinder : public object {
  public:
    cylinder(render::point const & pos, double r, render::vector const & ax, material::material mat)
        : object(pos, r, std::move(mat)), axis_{ax} { }

    // Getters
    [[nodiscard]] render::vector axis() const { return axis_; }

    [[nodiscard]] double height() const { return axis().magnitude(); }

    [[nodiscard]] render::vector normal(render::point const & ipoint) const override {
      render::vector const naxis  = axis().normalize();
      render::vector const v      = render::vector::get_vector_from_points(position(), ipoint);
      render::vector const normal = v - naxis * v.dot(naxis);
      return normal.normalize();
    }

    // Funciones para calculo de interseccion
    [[nodiscard]] object::paramsABC compute_params(ray::ray const & rayo) const override;

    static object::paramsABC compute_params_base(ray::ray const & rayo,
                                                 render::point const & centro_base,
                                                 render::vector const & normal_base);

    [[nodiscard]] bool is_in_cylinder_limits(render::vector a_hat, double lambda,
                                             ray::ray const & rayo) const;

  private:
    render::vector axis_;
  };

}  // namespace object

#endif
