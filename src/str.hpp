#include <string>
#include <vector>

[[gnu::pure]]
auto split_line(const std::string &line) -> std::vector<std::string>;
