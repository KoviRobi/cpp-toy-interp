#pragma once

/** \file
 * \brief Prints an AST
 */

#include "ast.hpp"
#include "eval.hpp"

#include <functional>

struct FmtAst : Visitor {
  FmtAst(std::function<void(const std::string &)> output);

  void visitAssignment(const Assignment &let);
  void visitFn(const Fn &fn);
  void visitApp(const App &app);
  void visitBinop(const Binop &op);
  void visitNumber(const Number &n);
  void visitIdentifier(const Identifier &id);

private:
  std::function<void(const std::string &)> output;
};

struct FmtValue : ValueVisitor {
  FmtValue(FmtAst &ast_visitor,
           std::function<void(const std::string &)> output);

  void visitNumber(const NumberValue &n) override;
  void visitClosure(const ClosureValue &c) override;

private:
  FmtAst &ast_visitor;
  std::function<void(const std::string &)> output;
};
