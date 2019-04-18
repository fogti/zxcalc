#include "cpm.hpp"
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <sstream>

using std::move;
using std::string;
using std::optional;

CalcPluginManager::plgdat_t::plgdat_t()
  : handle(RTLD_DEFAULT) { }

CalcPluginManager::CalcPluginManager() {
  // default aliases
  alias("_"    , "raw");
  alias("*"    , "mul");
  alias("2^"   , "base2");
  alias("%"    , "percent");
  alias("!%"   , "invnperc");
  alias("!+%"  , "invpperc");
  alias("!!%"  , "invaperc");
  parse_setup();
}

CalcPluginManager::~CalcPluginManager() {
  for(const auto &i : _plgs) {
    void * handle = i.second.handle;
    if(handle != RTLD_DEFAULT) dlclose(handle);
  }
}

bool CalcPluginManager::resolve(string &plgname) {
  {
    const auto it = _aliases.find(plgname);
    if(it != _aliases.end()) plgname = it->second;
  }
  return load(plgname);
}

bool CalcPluginManager::load(const string &plgname) {
  { // 1. check if already loaded
    const auto it = _plgs.find(plgname);
    if(it != _plgs.end()) return true;
  }
  auto &plg = _plgs[plgname];
  auto &handle = plg.handle;
  const string calc_fn_name = plgname + "_calc";
  { // 2. check if global available
    handle = RTLD_DEFAULT;
    if(dlsym(handle, calc_fn_name.c_str())) return true;
  }
  { // 3. check for plugin as shared-object
    const string pp = "./zxcalc.plugins/lib" + plgname + ".so";
    handle = dlopen(pp.c_str(), RTLD_NOW | RTLD_LOCAL);
  }
  if(handle) {
    if(dlsym(handle, calc_fn_name.c_str()))
      return true;
    dlclose(handle);
  }
  _plgs.erase(plgname);
  return false;
}

void * CalcPluginManager::plgsym(const string &plgname, const string &fname) {
  const string fn = plgname + '_' + fname;
  return dlsym(_plgs[plgname].handle, fn.c_str());
}

void CalcPluginManager::alias(const std::string &a, const std::string &plgname) {
  _aliases[a] = plgname;
}

void CalcPluginManager::parse_setup() {
  std::ifstream in("./zxcalc.plugins/aliases");
  string line;
  while(std::getline(in, line)) {
    if(line.empty()) continue;
    const size_t splp = line.find(';');
    if(splp != string::npos) line.erase(line.begin() + splp, line.end());
    if(line.empty()) continue;
    std::istringstream ss(line);
    string al, pl;
    if(!(ss >> al >> pl)) {
      std::cerr << "ERROR: parse_setup(./zxcalc.plugins/aliases): failed to parse line: '" << line << "'\n";
      continue;
    }
    alias(al, pl);
  }
}

void CalcPluginManager::list_loaded_plugins() const {
  using std::cout;
  for(const auto &i: _plgs)
    cout << '\t' << (i.second.handle ? 'e' : 'b') << ' ' << i.first << '\n';
  for(const auto &i: _aliases)
    cout << "\tA " << i.first << "\t= " << i.second << '\n';
}

/* plugin functions, prereq: resolve(plgname) */
auto CalcPluginManager::calc_intern(const string &plgname, const string &fname, const double x) -> optional<double> {
  double (*fn_calc)(double) = reinterpret_cast<decltype(fn_calc)>(plgsym(plgname, fname));
  return fn_calc ? optional<double>(fn_calc(x)) : std::nullopt;
}
