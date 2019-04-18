#include <optional>
#include <string>
#include <unordered_map>

class CalcPluginManager {
  struct plgdat_t {
    plgdat_t();
    void *handle;
  };

  std::unordered_map<std::string, plgdat_t> _plgs;
  std::unordered_map<std::string, std::string> _aliases;
  bool load(const std::string &plgname);
  void * plgsym(const std::string &plgname, const std::string &fname);

  auto calc_intern(const std::string &plgname, const std::string &fname, const double x) -> std::optional<double>;

 public:
  CalcPluginManager();
  ~CalcPluginManager();
  bool resolve(std::string &plgname);
  void alias(const std::string &a, const std::string &plgname);
  void parse_setup();
  void list_loaded_plugins() const;

  /* plugin functions, prereq: resolve(plgname) */
  auto calc(const std::string &plgname, const double x)
    { return calc_intern(plgname, "calc", x); }
  auto calcinv(const std::string &plgname, const double x)
    { return calc_intern(plgname, "calcinv", x); }
};
