#ifndef config_PARSER_HPP
#define config_PARSER_HPP

#include "image.hpp"
#include <string>

class configurationParser {
  // Clase para parsear archivos de configuración de la imagen
public:
  static imagec::imageConfig parse(std::string const & filename);

  static void process_line(std::string const & line, imagec::imageConfig & cfg);

  // Funciones para parsear cada clave individualmente
  static void parse_aspect_ratio(std::string const & line, imagec::imageConfig & image_config);
  static void parse_width(std::string const & line, imagec::imageConfig & image_config);
  static void parse_gamma(std::string const & line, imagec::imageConfig & image_config);
  static void parse_camera_position(std::string const & line, imagec::imageConfig & image_config);
  static void parse_camera_target(std::string const & line, imagec::imageConfig & image_config);
  static void parse_camera_north(std::string const & line, imagec::imageConfig & image_config);
  static void parse_field_of_view(std::string const & line, imagec::imageConfig & image_config);
  static void parse_samples_per_pixel(std::string const & line, imagec::imageConfig & image_config);
  static void parse_max_depth(std::string const & line, imagec::imageConfig & image_config);
  static void parse_material_rng_seed(std::string const & line, imagec::imageConfig & image_config);
  static void parse_ray_rng_seed(std::string const & line, imagec::imageConfig & image_config);
  static void parse_background_dark_color(std::string const & line,
                                          imagec::imageConfig & image_config);
  static void parse_background_light_color(std::string const & line,
                                           imagec::imageConfig & image_config);
};

#endif
