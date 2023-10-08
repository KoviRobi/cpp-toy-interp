#include "eval.hpp"
#include "formatter.hpp"
#include "parser.hpp"
#include "readline.hpp"
#include "tokeniser.hpp"

#include <iostream>
#include <memory>
#include <optional>

int main(int argc, char *argv[]) {
  Readline readline;
  std::optional<std::string> line;

  std::string formatted;
  EvalVisitor evaluator;
  FmtAst ast_formatter([&](auto s) { formatted += s; });
  FmtValue value_formatter(ast_formatter, [&](auto s) { formatted += s; });

  while ((line = readline.read("> ")).has_value()) {
    auto tree = parse(*line);
    for (auto &node : tree) {
      node->accept(evaluator);
    }
    formatted = "";
    if (evaluator.get_last()) {
      evaluator.get_last()->accept(value_formatter);
      std::cout << formatted << std::endl;
    }
  }
}
