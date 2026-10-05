#ifndef IMAGE_REP_HPP
#define IMAGE_REP_HPP

#include <string>
#include <vector>

namespace image {

  class imagePAR {
  public:
    imagePAR(int width, int height);

    // Getters para un pixel en conjunto
    [[nodiscard]] std::tuple<int, int, int> pixel(int row, int col) const;

    // Setters para un pixel en conjunto
    void setPixel(int row, int col, std::tuple<int, int, int> const & rgb);

    // Generar salida ppm
    void write_ppm(std::string const & filename) const;

    // Obtener las dimensiones de la imagen
    [[nodiscard]] int width() const { return width_; }

    [[nodiscard]] int height() const { return height_; }

  private:
    int width_, height_;

    struct Pixel {
      int r, g, b;
    };

    std::vector<Pixel> array_;
  };

}  // namespace image

#endif
