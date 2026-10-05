#include "../include/configuration_parser.hpp"
#include "../include/image.hpp"
#include "../include/parser.hpp"

#include "../include/scene_parser.hpp"
#include "../include/vector.hpp"
#include <cstddef>
#include <exception>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

void configurationParser::process_line(std::string const & line, imagec::imageConfig & cfg) {
  // Ignorar líneas en blanco
  if (parser::is_blank_line(line)) {
    return;
  }
  auto tokens = parser::split_line(line);  // Tokenizar la línea
  if (tokens.empty()) {
    return;
  }
  std::string const & key = tokens[0];  // Asumimos tokens en minúscula

  if (key == "aspect_ratio:") {
    configurationParser::parse_aspect_ratio(line, cfg);
  } else if (key == "image_width:") {
    configurationParser::parse_width(line, cfg);
  } else if (key == "gamma:") {
    configurationParser::parse_gamma(line, cfg);
  } else if (key == "camera_position:") {
    configurationParser::parse_camera_position(line, cfg);
  } else if (key == "camera_target:") {
    configurationParser::parse_camera_target(line, cfg);
  } else if (key == "camera_north:") {
    configurationParser::parse_camera_north(line, cfg);
  } else if (key == "field_of_view:") {
    configurationParser::parse_field_of_view(line, cfg);
  } else if (key == "samples_per_pixel:") {
    configurationParser::parse_samples_per_pixel(line, cfg);
  } else if (key == "max_depth:") {
    configurationParser::parse_max_depth(line, cfg);
  } else if (key == "material_rng_seed:") {
    configurationParser::parse_material_rng_seed(line, cfg);
  } else if (key == "ray_rng_seed:") {
    configurationParser::parse_ray_rng_seed(line, cfg);
  } else if (key == "background_dark_color:") {
    configurationParser::parse_background_dark_color(line, cfg);
  } else if (key == "background_light_color:") {
    configurationParser::parse_background_light_color(line, cfg);
  } else {
    throw std::invalid_argument(std::format("Unknown configuration key: [{}]", key));
  }
}

imagec::imageConfig configurationParser::parse(std::string const & filename) {
  // Comenzar a parsear el archivo de configuración
  imagec::imageConfig configuration;
  std::ifstream file(filename);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  std::string line;
  int line_number = 0;

  // Leer línea por línea
  while (std::getline(file, line)) {
    line_number++;
    try {
      process_line(line, configuration);
    } catch (std::exception const & e) {
      throw std::runtime_error("Error in line " + std::to_string(line_number) + ": " + e.what());
    }
  }

  file.close();
  return configuration;
}

void configurationParser::parse_aspect_ratio(std::string const & line,
                                             imagec::imageConfig & image_config) {
  // Parsear aspect ratio
  auto tokens = parser::split_line(line);

  if (tokens.size() < 3) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "aspect_ratio" : tokens[0], line));
  }
  if (tokens.size() > 3) {
    std::string extra;
    for (size_t i = 3; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const w = std::stoi(tokens[1]);
  int const h = std::stoi(tokens[2]);

  image_config.setResolution(w, h);
}

void configurationParser::parse_width(std::string const & line,
                                      imagec::imageConfig & image_config) {
  // Parsear width
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "width" : tokens[0], line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const w = std::stoi(tokens[1]);

  image_config.setWidth(w);
}

void configurationParser::parse_gamma(std::string const & line,
                                      imagec::imageConfig & image_config) {
  // Parsear gamma
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "gamma" : tokens[0], line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  double const g = std::stod(tokens[1]);

  image_config.setGamma(g);
}

void configurationParser::parse_camera_position(std::string const & line,
                                                imagec::imageConfig & image_config) {
  // Parsear camera position
  auto tokens = parser::split_line(line);

  if (tokens.size() < 4) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "camera_position" : tokens[0], line));
  }
  if (tokens.size() > 4) {
    std::string extra;
    for (size_t i = 4; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }

  render::point const position =
      render::point(std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3]));

  image_config.setCameraPosition(position);
}

void configurationParser::parse_camera_target(std::string const & line,
                                              imagec::imageConfig & image_config) {
  // Parsear camera target
  auto tokens = parser::split_line(line);

  if (tokens.size() < 4) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "camera_target" : tokens[0], line));
  }
  if (tokens.size() > 4) {
    std::string extra;
    for (size_t i = 4; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  render::point const target =
      render::point(std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3]));
  image_config.setCameraTarget(target);
}

void configurationParser::parse_camera_north(std::string const & line,
                                             imagec::imageConfig & image_config) {
  // Parsear camera north
  auto tokens = parser::split_line(line);

  if (tokens.size() < 4) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "camera_north" : tokens[0], line));
  }
  if (tokens.size() > 4) {
    std::string extra;
    for (size_t i = 4; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  render::vector const north =
      render::vector(std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3]));
  image_config.setCameraNorth(north);
}

void configurationParser::parse_field_of_view(std::string const & line,
                                              imagec::imageConfig & image_config) {
  // Parsear field of view
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "field_of_view" : tokens[0], line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const fov = std::stoi(tokens[1]);
  image_config.setFieldOfView(fov);
}

void configurationParser::parse_samples_per_pixel(std::string const & line,
                                                  imagec::imageConfig & image_config) {
  // Parsear samples per pixel
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "samples_per_pixel" : tokens[0],
                                            line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const s_p_p = std::stoi(tokens[1]);

  image_config.setSamplesPerPixel(s_p_p);
}

void configurationParser::parse_max_depth(std::string const & line,
                                          imagec::imageConfig & image_config) {
  // Parsear max depth
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "max_depth" : tokens[0], line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const m_d = std::stoi(tokens[1]);

  image_config.setMaxDepth(m_d);
}

void configurationParser::parse_material_rng_seed(std::string const & line,
                                                  imagec::imageConfig & image_config) {
  // Parsear material rng seed
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "material_rng_seed" : tokens[0],
                                            line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const m_s = std::stoi(tokens[1]);

  image_config.setMaterialSeed(m_s);
}

void configurationParser::parse_ray_rng_seed(std::string const & line,
                                             imagec::imageConfig & image_config) {
  // Parsear ray rng seed
  auto tokens = parser::split_line(line);

  if (tokens.size() < 2) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "ray_rng_seed" : tokens[0], line));
  }
  if (tokens.size() > 2) {
    std::string extra;
    for (size_t i = 2; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  int const r_s = std::stoi(tokens[1]);

  image_config.setRaySeed(r_s);
}

void configurationParser::parse_background_dark_color(std::string const & line,
                                                      imagec::imageConfig & image_config) {
  // Parsear background dark color
  auto tokens = parser::split_line(line);

  if (tokens.size() < 4) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "background_dark_color" : tokens[0],
                                            line));
  }
  if (tokens.size() > 4) {
    std::string extra;
    for (size_t i = 4; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }
  image_config.setBackgroundDark(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void configurationParser::parse_background_light_color(std::string const & line,
                                                       imagec::imageConfig & image_config) {
  // Parsear background light color
  auto tokens = parser::split_line(line);

  if (tokens.size() < 4) {
    throw std::invalid_argument(std::format("Error: Invalid value for key: [{}:]\nLine: \"{}\"",
                                            tokens.empty() ? "background_light_color" : tokens[0],
                                            line));
  }
  if (tokens.size() > 4) {
    std::string extra;
    for (size_t i = 4; i < tokens.size(); ++i) {
      if (!extra.empty()) {
        extra += " ";
      }
      extra += tokens[i];
    }

    throw std::invalid_argument(
        std::format("Error: Extra data after configuration value for key: [{}:]\nExtra: \"{}\"",
                    tokens[0], extra));
  }

  image_config.setBackgroundLight(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}
