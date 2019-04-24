#include "zxcalc_config.h"

#ifdef LIBEDIT_FOUND
extern "C" {
# include <histedit.h>
}
#endif
#include <stdio.h>

#include "cpm.hpp"
#include "str.hpp"
#include "zxmath.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <forward_list>
#include <algorithm>
#include <utility>

using namespace std;

struct x_node_t {
  string clp, var;
  double val;
  char st; // one of: +-*/:=e

  static auto expr(string clp, string var, double val, char st) -> x_node_t {
    return x_node_t{ std::move(clp), std::move(var), val, st };
  }

  static auto trexpr(string x, char st) -> x_node_t {
    return x_node_t{ std::move(x), string(), 0.0, st };
  }

  static auto error(string x) -> x_node_t {
    return x_node_t{ std::move(x), string(), 0.0, 'e' };
  }
};

template<typename T>
static bool is_in_uvec(const vector<T> &vec, const T& val) noexcept {
  const auto ie = vec.end();
  const auto it = std::lower_bound(vec.begin(), ie, val);
  return (it != ie && *it == val);
}

static auto parse_line(const string &line, const double prev_val, const vector<string> cmds2passup) -> vector<x_node_t> {
  auto toks = split_line(line);
  if(toks.empty()) return {};
  vector<x_node_t> nodes;

  {
    auto &i = toks.front();
    if(toks.size() == 1 && is_in_uvec(cmds2passup, i))
      return {x_node_t::error(move(i))};

    if(i.size() != 1 || i.find_first_of("+-*/:=") == string::npos) {
      nodes.emplace_back(x_node_t::expr(move(i), string(), prev_val, '+'));
      toks.erase(toks.begin());
    }
  }

  enum { PM_START, PM_CLP, PM_CLPX, PM_NUM } mode = PM_START;
  char st = '+';
  string clp;
  for(auto &&i : toks) {
    switch(mode) {
      case PM_START:
        if(i.size() == 1) {
          st = i.back();
          switch(st) {
            case '+': case '-':
            case '*': case '/':
            case '%':
              mode = PM_CLP ; continue;

            case ':': case '=':
              mode = PM_CLPX; continue;

            default:
              break;
          }
        }
        nodes.emplace_back(x_node_t::error("expected one of '+'|'-'|'*'|'/'|':'|'=' instead of '" + move(i) + "'"));
        break;
      case PM_CLP: // expect calc plugin name
        clp = move(i);
        mode = PM_NUM;
        break;
      case PM_CLPX: // expect calc plugin name
        nodes.emplace_back(x_node_t::trexpr(move(i), st));
        mode = PM_START;
        break;
      case PM_NUM:
        mode = PM_START;
        {
          double val = 0.0;
          bool is_num = false;
          try {
            val = stod(i);
            is_num = true;
          } catch(...) {
            val = 0.0;
          }
          nodes.emplace_back(x_node_t::expr(move(clp), is_num ? string() : move(i), val, st));
        }
        break;
    }
    if(!nodes.empty() && nodes.back().st == 'e') break;
  }
  string xx;
  switch(mode) {
    case PM_START: break;
    case PM_CLP:
    case PM_CLPX: xx = "calc plugin name"; break;
    case PM_NUM:  xx = "number"; break;
  }
  if(!xx.empty()) nodes.emplace_back(x_node_t::error("unexpected EOL while looking for " + xx));
  return nodes;
}

[[gnu::const]]
static double eval_op(double y, const double x, const char op) {
  switch(op) {
    case '+': y += x; break;
    case '-': y -= x; break;
    case '*': y *= x; break;
    case '/': y /= x; break;
    case '%': y = zx_modulo(y, x); break;
  }
  return y;
}

#ifdef LIBEDIT_FOUND
static const char * prompt(EditLine *e) {
  return "zxcalc $ ";
}
#endif

int main() {
  const vector<string> cmds2passup = { "exit", "help", "list-loaded-plugins", "quit" };
  CalcPluginManager cpm;
  unordered_map<string, double> vars;
  string line;
  double value = 0;
  bool breakout = false;

#ifdef LIBEDIT_FOUND
  EditLine *el;
  History *myhistory;

  /* Temp variables */
  int count;
  const char *line_c;
  HistEvent ev;

  el = el_init("zxcalc", stdin, stdout, stderr);
  el_set(el, EL_PROMPT, &prompt);
  el_set(el, EL_EDITOR, "emacs");

  myhistory = history_init();
  if(!myhistory) {
    cerr << "history could not be initialized\n";
    return 1;
  }

  history(myhistory, &ev, H_SETSIZE, 800);
  el_set(el, EL_HIST, history, myhistory);

  while(!breakout) {
    line_c = el_gets(el, &count);
    if(!count) continue;
    history(myhistory, &ev, H_ENTER, line_c);
    line = line_c;
#else
  while(!breakout && getline(cin, line)) {
#endif
    auto parts = parse_line(line, value, cmds2passup);
    vars["_"] = value;
    value = 0;
    bool got_error = false;
    for(auto &i : parts) {
      if(i.st != 'e' && i.st != '=' && !cpm.resolve(i.clp)) {
        got_error = true;
        cerr << "\tERROR: " << i.clp << ": unable to resolve plugin name\n";
        break;
      }
      switch(i.st) {
        case 'e':
          got_error = true;
          if(i.clp == "quit" || i.clp == "exit") {
            breakout = true;
            break;
          } else if(i.clp == "list-loaded-plugins") {
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
              "\t\t(('+'|'-'|'*'|'/') PLG ['+'|'-']NUM|':' PLG|'=' VAR)*\n\n";
            break;
          }
          cerr << "\tERROR: " << i.clp << '\n';
          break;

        case ':':
          if(const auto res = cpm.calcinv(i.clp, value)) {
            value = *res;
          } else {
            got_error = true;
            cerr << "\tERROR: " << i.clp << " " << value << ": calcinv failed\n";
          }
          break;

        case '=':
          vars[i.clp] = value;
          break;

        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
          if(!i.var.empty()) {
            const auto it = vars.find(i.var);
            if(it == vars.end()) {
              got_error = true;
              cerr << "\tERROR: " << i.var << ": unknown variable\n";
              break;
            }
            i.val = it->second;
          }
          if(const auto res = cpm.calc(i.clp, i.val)) {
            value = eval_op(value, *res, i.st);
          } else {
            got_error = true;
            cerr << "\tERROR: " << i.clp << " " << i.val << ": calc failed\n";
          }
          break;

        default:
          got_error = true;
          cerr << "\tINTERNAL ERROR: unimplemented operation '" << i.st << "'\n";
          break;
      }
      if(breakout || got_error) break;
    }
    if(!got_error) cout << '\t' << value << '\n';
  }

#ifdef LIBEDIT_FOUND
  history_end(myhistory);
  el_end(el);
#endif

  return 0;
}
