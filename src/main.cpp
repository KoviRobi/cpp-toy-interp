#include "formatter.hpp"
#include "parser.hpp"
#include "readline.hpp"
#include "tokeniser.hpp"

#include <iostream>
#include <optional>

int main(int argc, char *argv[]) {
  Readline readline;
  std::optional<std::string> line;
  while ((line = readline.read("> ")).has_value()) {
    auto tree = parse(*line);
    auto formatter = FmtVisitor();
    for (auto &node : tree) {
      node->accept(formatter);
      std::cout << *formatter << std::endl;
      formatter.clear();
    }
  }
}
