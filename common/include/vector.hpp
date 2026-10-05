#ifndef RENDER_VECTOR_HPP
#define RENDER_VECTOR_HPP

#include <cmath>
#include <stdexcept>

namespace render {

  class point {
    // Clase punto
  public:
    point(double cx, double cy, double cz) : x{cx}, y{cy}, z{cz} { }

    [[nodiscard]] double X() const { return x; }

    [[nodiscard]] double Y() const { return y; }

    [[nodiscard]] double Z() const { return z; }

  private:
    double x, y, z;
  };

  class vector {
  public:
    vector(double cx, double cy, double cz) : x{cx}, y{cy}, z{cz} { }

    // Getters
    [[nodiscard]] double X() const { return x; }

    [[nodiscard]] double Y() const { return y; }

    [[nodiscard]] double Z() const { return z; }

    // Funciones útiles
    [[nodiscard]] double magnitude() const { return std::sqrt(x * x + y * y + z * z); }

    [[nodiscard]] vector normalize() const {
      double const mag = magnitude();
      if (mag < 1e-12 or std::isnan(mag)) {
        // Devolver vector por defecto en lugar de lanzar excepción
        return {1.0, 0.0, 0.0};
      }
      return {x / mag, y / mag, z / mag};
    }

    [[nodiscard]] double dot(vector const & other) const {
      return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] vector cross(vector const & other) const {
      return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

    [[nodiscard]] vector add(double scalar) const { return {x + scalar, y + scalar, z + scalar}; }

    // Operadores
    friend vector operator+(vector const & a, vector const & b) {
      return {a.X() + b.X(), a.Y() + b.Y(), a.Z() + b.Z()};
    }

    friend vector operator-(vector const & a, vector const & b) {
      return {a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z()};
    }

    friend point operator-(point const & a, vector const & b) {
      return {a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z()};
    }

    friend vector operator*(vector const & v, double scalar) {
      return {v.X() * scalar, v.Y() * scalar, v.Z() * scalar};
    }

    friend vector operator/(vector const & v, double scalar) {
      if (scalar == 0) {
        throw std::runtime_error("Cannot divide by zero");
      }
      return {v.X() / scalar, v.Y() / scalar, v.Z() / scalar};
    }

    static vector get_vector_from_points(point const & a, point const & b) {
      return {b.X() - a.X(), b.Y() - a.Y(), b.Z() - a.Z()};
    }

    friend point operator+(point const & p, vector const & v) {
      return {p.X() + v.X(), p.Y() + v.Y(), p.Z() + v.Z()};
    }

    friend vector operator-(vector const & v) { return {-v.X(), -v.Y(), -v.Z()}; }

  private:
    double x, y, z;
  };

}  // namespace render

#endif
