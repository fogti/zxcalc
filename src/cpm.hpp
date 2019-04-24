#include <stddef.h>
#include <optional>
#include <string>
#include <unordered_map>

class CalcPluginManager {
  struct plgdat_t {
    plgdat_t();
    void *handle;
    size_t scale;
  };

  std::unordered_map<std::string, plgdat_t> _plgs;
  std::unordered_map<std::string, std::string> _aliases;

  void * plgsym(const std::string &plgname, const std::string &fname);
  auto calc_intern(const std::string &plgname, bool is_inv, double x) -> std::optional<double>;

 public:
  CalcPluginManager();
  ~CalcPluginManager();
  bool resolve(std::string &plgname);
  void alias(const std::string &a, std::string plgname);
  void parse_setup();
  void list_loaded_plugins() const;

  /* plugin functions, prereq: resolve(plgname) */
  void set_scale(const std::string &plgname, size_t scale);
  auto calc(const std::string &plgname, const double x)
    { return calc_intern(plgname, false, x); }
  auto calcinv(const std::string &plgname, const double x)
    { return calc_intern(plgname, true, x); }
};
