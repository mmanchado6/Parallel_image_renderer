#include "../include/parser.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>

namespace parser {

  std::vector<std::string> split_line(std::string const & line) {
    std::vector<std::string> tokens;

    if (line.empty()) {
      return tokens;
    }

    size_t start        = 0;
    size_t end          = 0;
    size_t const length = line.length();

    // Encontrar el primer token
    while (start < length and parser::is_whitespace(line[start])) {
      start++;
    }

    // Si toda la línea es espacio en blanco, retornar vector vacío
    if (start >= length) {
      return tokens;
    }

    // Recorrer la línea para extraer tokens
    while (start < length) {
      // Encontrar el final del token actual
      end = start;
      while (end < length and !parser::is_whitespace(line[end])) {
        end++;
      }

      // Extraer el token
      std::string const token = line.substr(start, end - start);
      tokens.push_back(token);

      // Avanzar más allá de los espacios
      start = end;
      while (start < length and parser::is_whitespace(line[start])) {
        start++;
      }
    }
    return tokens;
  }

  bool is_blank_line(std::string const & line) {
    return std::ranges::all_of(line, [](char c) { return is_whitespace(c); });
  }

}  // namespace parser
