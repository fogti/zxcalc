#include "cpm.hpp"
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <sstream>

using std::move;
using std::string;
using std::optional;

CalcPluginManager::plgdat_t::plgdat_t()
  : handle(RTLD_DEFAULT), scale(1) { }

CalcPluginManager::CalcPluginManager() {
  // default aliases
  alias("_"  , "raw");
  alias("2^" , "base2");
  alias("10^", "base10");
  alias("%"  , "percent");
  alias("!%" , "invnperc");
  alias("!+%", "invpperc");
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

  { // 1. check if already loaded
    const auto it = _plgs.find(plgname);
    if(it != _plgs.end()) return true;
  }
  auto &handle = _plgs[plgname].handle;
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

void CalcPluginManager::alias(const string &a, string plgname) {
  _aliases[a] = move(plgname);
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
    alias(al, move(pl));
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
void CalcPluginManager::set_scale(const string &plgname, size_t scale) {
  if(!scale) scale = 1;
  _plgs[plgname].scale = scale;
}

auto CalcPluginManager::calc_intern(const string &plgname, const bool is_inv, double x) -> optional<double> {
  typedef double (*calc_fnt)(double);
  const string fname = is_inv ? "calcinv" : "calc";
  calc_fnt fn_calc = reinterpret_cast<calc_fnt>(plgsym(plgname, fname));
  if(!fn_calc) return std::nullopt;
  double scale = 1;
  {
    const size_t tmp_scale = _plgs[plgname].scale;
    scale = (tmp_scale > 0) ? tmp_scale : (-1.0 / tmp_scale);
  }
  double tmp = x;
  if(is_inv)  tmp /= scale;
  tmp = fn_calc(tmp);
  if(!is_inv) tmp *= scale;
  return optional<double>(tmp);
}
