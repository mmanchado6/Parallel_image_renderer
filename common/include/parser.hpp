#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#include <vector>

namespace parser {

  std::vector<std::string> split_line(std::string const & line);
  bool is_blank_line(std::string const & line);

  inline bool is_whitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
  }

};  // namespace parser
#endif
