#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "vector.hpp"
#include <random>
#include <stdexcept>
#include <tuple>

#include <cmath>

namespace imagec {

  using Color = std::tuple<double, double, double>;

  struct image_config_params {
    std::tuple<int, int> resolution;
    int width;
    double gamma;
    std::tuple<render::point, render::point, render::vector, int> params_view;
    int samples_per_pixel;
    int max_depth;
    int material_seed;
    int ray_seed;
    Color background_dark;
    Color background_light;
  };

  class imageConfig {
  public:
    imageConfig(image_config_params const & params = {{16, 9},
                                                      1'920,
                                                      2.2,
                                                      {render::point(0.0, 0.0, -10.0),
                                                       render::point(0.0, 0.0, 0.0),
                                                       render::vector(0.0, 1.0, 0.0), 90},
                                                      20,
                                                      5,
                                                      13,
                                                      19,
                                                      {0.25, 0.5, 1.0},
                                                      {1.0, 1.0, 1.0}})
        : rs{params.resolution}, w{params.width}, g{params.gamma}, pv{params.params_view},
          spp{params.samples_per_pixel}, mdepth{params.max_depth}, mseed{params.material_seed},
          rseed{params.ray_seed}, bdark{params.background_dark}, blight{params.background_light} { }

    // Getters
    [[nodiscard]] std::tuple<int, int> resolution() const { return rs; }

    [[nodiscard]] int width() const { return w; }

    [[nodiscard]] double gamma() const { return g; }

    [[nodiscard]] std::tuple<render::point, render::point, render::vector, int> paramsView() const {
      return pv;
    }

    [[nodiscard]] int samples_per_pixel() const { return spp; }

    [[nodiscard]] int max_depth() const { return mdepth; }

    [[nodiscard]] int material_seed() const { return mseed; }

    [[nodiscard]] int ray_seed() const { return rseed; }

    [[nodiscard]] Color background_dark() const { return bdark; }

    [[nodiscard]] Color background_light() const { return blight; }

    [[nodiscard]] int height() const { return w * std::get<1>(rs) / std::get<0>(rs); }

    [[nodiscard]] static std::mt19937_64 & get_material_gen() { return material_gen; }

    // Setters

    void setResolution(int w_aspect, int h_aspect) {
      if (w_aspect <= 0 or h_aspect <= 0) {
        throw std::invalid_argument("Error:Resolution dimensions must be positive integers.");
      }
      rs = std::make_tuple(w_aspect, h_aspect);
    }

    void setWidth(int width_) {
      if (width_ <= 0) {
        throw std::invalid_argument("Error: Width must be a positive integer.");
      }
      w = width_;
    }

    void setGamma(double gamma_) {
      if (gamma_ <= 0) {
        throw std::invalid_argument("Error: Gamma must be a positive float.");
      }
      g = gamma_;
    }

    void setCameraPosition(render::point const & pos) { std::get<0>(pv) = pos; }

    void setCameraTarget(render::point const & target) { std::get<1>(pv) = target; }

    void setCameraNorth(render::vector const & north) { std::get<2>(pv) = north; }

    void setFieldOfView(int fov) { std::get<3>(pv) = fov; }

    void setSamplesPerPixel(int samples) {
      if (samples <= 0) {
        throw std::invalid_argument("Error: Samples per pixel must be a positive integer.");
      }
      spp = samples;
    }

    void setMaxDepth(int d) {
      if (d < 0) {
        throw std::invalid_argument("Error: Max depth must be a non-negative integer.");
      }
      mdepth = d;
    }

    void setMaterialSeed(int seed) { mseed = seed; }

    void setRaySeed(int seed) { rseed = seed; }

    void setBackgroundDark(double r, double g_, double b) {
      if (r < 0.0F or r > 1.0F or g_ < 0.0F or g_ > 1.0F or b < 0.0F or b > 1.0F) {
        throw std::invalid_argument(
            "Error: Background color components must be in the range [0, 1].");
      }

      bdark = std::make_tuple(r, g_, b);
    }

    void setBackgroundLight(double r, double g_, double b) {
      if (r < 0.0F or r > 1.0F or g_ < 0.0F or g_ > 1.0F or b < 0.0F or b > 1.0F) {
        throw std::invalid_argument(
            "Error: Background color components must be in the range [0, 1].");
      }

      blight = std::make_tuple(r, g_, b);
    }

    void validate() const;

    static void init_material_rng(std::mt19937_64::result_type seed);

  private:
    std::tuple<int, int> rs;
    int w;
    double g;
    std::tuple<render::point, render::point, render::vector, int> pv;
    int spp;
    int mdepth;
    int mseed;
    int rseed;
    Color bdark;
    Color blight;
    static std::mt19937_64 material_gen;
  };

  class Pov {
  public:
    // Constructor
    Pov(render::point const & pos    = render::point(0, 0, -10),
        render::point const & dest   = render::point(0, 0, 0),
        render::vector const & north = render::vector(0, 1, 0), double field_view = 90.0,
        int image_width = 1'920, int image_heigth = 1'080,
        std::uint_fast64_t seed = std::random_device{}())
        : pos{pos}, dst{dest}, n{north}, fview{field_view}, iwidth{image_width},
          iheigth{image_heigth}, sd{seed}, gen{seed} { }

    // Getters
    [[nodiscard]] render::point position() const { return pos; }

    [[nodiscard]] render::point destination() const { return dst; }

    [[nodiscard]] render::vector north() const { return n; }

    [[nodiscard]] double field_view() const { return fview; }

    [[nodiscard]] int image_width() const { return iwidth; }

    [[nodiscard]] int image_heigth() const { return iheigth; }

    [[nodiscard]] std::uint_fast64_t seed() const { return sd; }

    [[nodiscard]] render::vector focal_vector() const {
      return render::vector{pos.X() - dst.X(), pos.Y() - dst.Y(), pos.Z() - dst.Z()};
    }

    [[nodiscard]] double focal_distance() const { return focal_vector().magnitude(); }

    [[nodiscard]] double heigth_proj_win() const {
      double const alpha_rad = field_view() * std::numbers::pi / 180.0;
      return 2.0 * std::tan(alpha_rad / 2.0) * focal_distance();
    }

    [[nodiscard]] double width_proj_win() const {
      return heigth_proj_win() * (double(iwidth) / double(iheigth));
    }

    [[nodiscard]] render::vector dirv_u() const;

    [[nodiscard]] render::vector dirv_v() const {
      return focal_vector().normalize().cross(dirv_u());
    }

    [[nodiscard]] render::vector phorizontal() const { return dirv_u() * width_proj_win(); }

    [[nodiscard]] render::vector pvertical() const { return -dirv_v() * heigth_proj_win(); }

    [[nodiscard]] render::vector displacement_x() const { return phorizontal() / iwidth; }

    [[nodiscard]] render::vector displacement_y() const { return pvertical() / iheigth; }

    [[nodiscard]] render::point origin_proj_win() const {
      render::vector const half_proj_view = (phorizontal() + pvertical()) / 2;
      render::vector const half_desp      = (displacement_x() + displacement_y()) / 2;

      return pos + (-focal_vector()) + (-half_proj_view) + half_desp;
    }

    // Acceso al generator aleatorio
    int random_generator() {
      // Generador de números aleatorios
      std::uniform_int_distribution<int> dist(0, std::numeric_limits<uint_fast64_t>::digits);
      return dist(gen);
    }

  private:
    render::point pos;      // P ≡ (px, py, pz)
    render::point dst;      // D ≡ (cx, cy, cz)
    render::vector n;       // n ≡ (nx, ny, nz)
    double fview;           // Ángulo en grados
    int iwidth;             // Tamaño en píxeles
    int iheigth;            // Tamaño en píxeles
    std::uint_fast64_t sd;  // seed para RNG
    std::mt19937_64 gen;    // generator Mersenne-Twister
  };

}  // namespace imagec

#endif
