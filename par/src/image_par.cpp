#include <cstddef>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <tuple>

#include "../include/image_par.hpp"

namespace image {

  imagePAR::imagePAR(int width, int height)
      : width_(width), height_(height),
        array_(static_cast<size_t>(width) * static_cast<size_t>(height), Pixel{0, 0, 0}) { }

  std::tuple<int, int, int> imagePAR::pixel(int row, int col) const {
    size_t const index =
        static_cast<size_t>(row) * static_cast<size_t>(width_) + static_cast<size_t>(col);

    const struct Pixel p = array_[index];
    return std::make_tuple(p.r, p.g, p.b);
  }

  void imagePAR::setPixel(int row, int col, std::tuple<int, int, int> const & rgb) {
    size_t const index =
        static_cast<size_t>(row) * static_cast<size_t>(width_) + static_cast<size_t>(col);

    struct Pixel p{};
    p.r           = std::get<0>(rgb);
    p.g           = std::get<1>(rgb);
    p.b           = std::get<2>(rgb);
    array_[index] = p;
  }

  void imagePAR::write_ppm(std::string const & filename) const {
    std::ofstream file(filename, std::ios::out bitor std::ios::trunc);
    if (!file.is_open()) {
      throw std::runtime_error("Cannot open file: " + filename);
    }

    file << "P3\n" << width_ << " " << height_ << "\n255\n";

    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
        auto [r, g, b] = pixel(y, x);

        file << r << " " << g << " " << b << "\n";
      }
    }

    file.close();
  }

}  // namespace image
