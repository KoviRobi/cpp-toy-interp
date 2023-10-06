#include "eval.hpp"
#include "formatter.hpp"
#include "parser.hpp"
#include "readline.hpp"
#include "tokeniser.hpp"

#include <iostream>
#include <memory>
#include <optional>

struct FmtValue : ValueVisitor {
  FmtValue(FmtVisitor &ast_visitor) : ast_visitor(ast_visitor) {}

  void visitNumber(const NumberValue &n) override {
    std::cout << n.get_value() << std::endl;
  }

  void visitClosure(const ClosureValue &c) override {
    std::cout << "fn ";
    bool first = true;
    for (auto &arg : c.get_args()) {
      if (!first)
        std::cout << " , ";
      first = false;
      std::cout << *arg;
    }

    std::cout << " { ";
    first = true;
    for (auto &body : c.get_body()) {
      if (!first)
        std::cout << " ; ";
      first = false;
      ast_visitor.clear();
      body->accept(ast_visitor);
      std::cout << *ast_visitor;
    }
    std::cout << " }" << std::endl;
  }

private:
  FmtVisitor &ast_visitor;
};

int main(int argc, char *argv[]) {
  Readline readline;
  std::optional<std::string> line;

  EvalVisitor evaluator;
  FmtVisitor formatter;
  FmtValue value_formatter(formatter);

  while ((line = readline.read("> ")).has_value()) {
    auto tree = parse(*line);
    for (auto &node : tree) {
      node->accept(evaluator);
    }
    formatter.clear();
    evaluator.get_last()->accept(value_formatter);
  }
}
