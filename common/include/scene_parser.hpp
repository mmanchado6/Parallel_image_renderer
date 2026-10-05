#include <string>

#include "scene.hpp"

class sceneParser {
public:
  static render::scene parse(std::string const & filename);

  static void process_line(std::string const & line, render::scene & scene);
  static void parse_material(std::string const & line, render::scene & scene);
  static void parse_sphere(std::string const & line, render::scene & scene);
  static void parse_cylinder(std::string const & line, render::scene & scene);
};
