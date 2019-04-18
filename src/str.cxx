#include "str.hpp"
#include <iostream>
#include <sstream>
#include <utility>

using std::string;
using std::vector;

auto split_line(const string &line) -> vector<string> {
  vector<string> parts;
  std::istringstream ss(line);
  string tok;
  while((ss >> tok))
    parts.emplace_back(std::move(tok));
  return parts;
}
