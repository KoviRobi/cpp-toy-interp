#include "parser.hpp"
#include "tokeniser.hpp"

#include <iostream>
#include <optional>

struct PrintVisitor : Visitor {
  void visitAssignment(Assignment &) { std::cout << "Assignment" << std::endl; }
  void visitNumExpr(NumExpr &n) {
    std::cout << "Number " << n.get() << std::endl;
  }
};

int main(int argc, char *argv[]) {
  const std::string hello = "Hello, world";
  auto tokeniser = Tokeniser(hello);
  std::optional<std::string> tok;
  while ((tok = tokeniser.next_token())) {
    std::cout << "Token: \"" << *tok << "\"" << std::endl;
  }

  auto tree = parse("");
  auto print_visitor = PrintVisitor();
  for (auto &node : tree) {
    node->accept(print_visitor);
  }
}
