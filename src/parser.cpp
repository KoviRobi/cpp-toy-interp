#include "parser.hpp"

std::vector<std::unique_ptr<Ast>> parse(const std::string &str) {
  std::vector<std::unique_ptr<Ast>> ret;
  ret.push_back(std::make_unique<NumExpr>(0));
  return ret;
}
