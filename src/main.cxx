#include "cpm.hpp"
#include "str.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include <forward_list>
#include <algorithm>
#include <utility>

using namespace std;

struct x_node_t {
  string clp;
  double val;
  enum st_t { CALC, MULCALC, DIVCALC, CALCINV, ERROR } st;

  static auto expr(string clp, double val, st_t st) -> x_node_t {
    return x_node_t{ std::move(clp), val, st };
  }

  static auto exprinv(string clp) -> x_node_t {
    return x_node_t{ std::move(clp), 0.0, CALCINV };
  }

  static auto error(string x) -> x_node_t {
    return x_node_t{ std::move(x), 0.0, ERROR };
  }
};

static auto parse_line(const string &line, const double prev_val) -> vector<x_node_t> {
  auto toks = split_line(line);
  if(toks.empty()) return {};
  vector<x_node_t> nodes;

  {
    auto &i = toks.front();
    if(toks.size() == 1)
      if(i == "quit" || i == "list-loaded-plugins" || i == "help")
        return {x_node_t::error(move(i))};

    if(i.size() != 1 || i.find_first_of("+-:") == string::npos) {
      nodes.emplace_back(x_node_t::expr(move(i), prev_val, x_node_t::CALC));
      toks.erase(toks.begin());
    }
  }

  enum { PM_START, PM_CLP, PM_CLPX, PM_NUM } mode = PM_START;
  bool is_neg = false;
  x_node_t::st_t st = x_node_t::CALC;
  string clp;
  for(auto &&i : toks) {
    switch(mode) {
      case PM_START: // expect '+' or '-'
        mode = PM_CLP;
        st = x_node_t::CALC;
        if(i.size() == 2) {
          switch(i.back()) {
            case '*': st = x_node_t::MULCALC; break;
            case '/': st = x_node_t::DIVCALC; break;
            default:  st = x_node_t::ERROR; break;
          }
        }
        if(st == x_node_t::ERROR) {
          nodes.emplace_back(x_node_t::error("expected one of '*'|'/' instead of '" + string(1, i.back()) + "'"));
          break;
        }
        if(i == ":") {
          mode = PM_CLPX;
          continue;
        } else if(st != x_node_t::CALC || i.size() == 1) {
          switch(i.front()) {
            case '+': is_neg = false; continue;
            case '-': is_neg = true;  continue;
          }
        }
        nodes.emplace_back(x_node_t::error("expected one of '+'|'-'|':' instead of '" + move(i) + "'"));
        break;
      case PM_CLP: // expect calc plugin name
        clp = move(i);
        mode = PM_NUM;
        break;
      case PM_CLPX: // expect calc plugin name
        nodes.emplace_back(x_node_t::exprinv(move(i)));
        mode = PM_START;
        break;
      case PM_NUM:
        mode = PM_START;
        {
          double val = 0.0;
          try {
            val = stod(i);
          } catch(...) {
            nodes.emplace_back(x_node_t::error("expected number instead of '" + move(i) + "'"));
            break;
          }
          nodes.emplace_back(x_node_t::expr(move(clp), is_neg ? (-val) : (val), st));
        }
        break;
    }
    if(!nodes.empty() && nodes.back().st == x_node_t::ERROR) break;
  }
  if(mode != PM_START) {
    string xx;
    switch(mode) {
      case PM_CLP:
      case PM_CLPX: xx = "calc plugin name"; break;
      case PM_NUM:  xx = "number"; break;
    }
    nodes.emplace_back(x_node_t::error("unexpected EOL while looking for " + xx));
  }
  return nodes;
}

static auto calc_wrapper(CalcPluginManager &cpm, bool &got_error, const x_node_t &i) {
  const auto res = cpm.calc(i.clp, i.val);
  if(!res) {
    got_error = true;
    cerr << "\tERROR: " << i.clp << " " << i.val << ": calc failed\n";
  }
  return res;
}

int main() {
  CalcPluginManager cpm;
  string line;
  double value = 0;

  while(getline(cin, line)) {
    auto parts = parse_line(line, value);
    value = 0;
    bool got_error = false;
    for(auto &i : parts) {
      if(i.st != x_node_t::ERROR && !cpm.resolve(i.clp)) {
        got_error = true;
        cerr << "\tERROR: " << i.clp << ": unable to resolve plugin name\n";
        break;
      }
      switch(i.st) {
        case x_node_t::ERROR:
          got_error = true;
          if(i.clp == "quit") return 0;
          else if(i.clp == "list-loaded-plugins") {
            cpm.list_loaded_plugins();
            break;
          } else if(i.clp == "help") {
            cout << "  --COMMANDS--\n"
              "\tquit\t\t\texit this program\n"
              "\thelp\t\t\tprint this text\n"
              "\tlist-loaded-plugins\tprint list of currently loaded plugins\n"
              "\n  --SYNTAX--\n"
              "\tThis program expects input lines not containing one of the\n"
              "\tcommands above to have the following format:\n"
              "\t\t(('+'|'-')(''|'*'|'/') PLG NUM|':' PLG)*\n\n";
            break;
          }
          cerr << "\tERROR: " << i.clp << '\n';
          break;

        case x_node_t::CALC:
          if(const auto res = calc_wrapper(cpm, got_error, i))
            value += *res;
          break;

        case x_node_t::MULCALC:
          if(const auto res = calc_wrapper(cpm, got_error, i))
            value *= *res;
          break;

        case x_node_t::DIVCALC:
          if(const auto res = calc_wrapper(cpm, got_error, i))
            value /= *res;
          break;

        case x_node_t::CALCINV:
          if(const auto res = cpm.calcinv(i.clp, value)) {
            value = *res;
          } else {
            got_error = true;
            cerr << "\tERROR: " << i.clp << " " << value << ": calcinv failed\n";
          }
          break;
      }
      if(got_error) break;
    }
    if(!got_error) cout << '\t' << value << '\n';
  }

  return 0;
}
