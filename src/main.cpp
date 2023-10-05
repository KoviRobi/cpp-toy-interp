#include "formatter.hpp"
#include "parser.hpp"
#include "tokeniser.hpp"

#include <iostream>
#include <optional>

int main(int argc, char *argv[]) {
  auto tree = parse("let x = 1");
  auto formatter = FmtVisitor();
  for (auto &node : tree) {
    node->accept(formatter);
    std::cout << *formatter << std::endl;
  }
}
