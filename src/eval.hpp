#pragma once

/** \file
 * \brief Contains values in the language (numbers and closures) and an
 * evaluator for evaluating a syntax-tree. Function-bodies live beyond their
 * AST, consider
 *
 *    int main(int argc, char *argv[]) {
 *      Readline readline;
 *      std::optional<std::string> line;
 *
 *      EvalVisitor evaluator;
 *      FmtVisitor formatter;
 *      FmtValue value_formatter(formatter);
 *
 *      while ((line = readline.read("> ")).has_value()) {
 *        auto tree = parse(*line);
 *        for (auto &node : tree) {
 *          node->accept(evaluator);
 *        }
 *        formatter.clear();
 *        evaluator.get_last()->accept(value_formatter);
 *      }
 *    }
 *
 * If you do `let id = fn x { x }` in one line/evaluation, the AST will get
 * deallocated in the next while loop iteration. But you still want to be able
 * to run `id 2` in another line/evaluation, so we use shared_ptr for function
 * bodies.
 */

#include "ast.hpp"

#include <map>
#include <vector>

struct EvalError : std::exception {
  const char *what(void) const noexcept override;
};

struct NotANumber : EvalError {
  const char *what(void) const noexcept override;
};

struct NotAFunction : EvalError {
  const char *what(void) const noexcept override;
};

struct UnknownVariable : EvalError {
  const char *what(void) const noexcept override;
};

struct ValueVisitor;
struct EvalVisitor;

struct Value {
  virtual void accept(ValueVisitor &) const = 0;
};

struct NumberValue : Value {
  NumberValue(uint32_t value);
  void accept(ValueVisitor &) const override;
  uint32_t get_value() const;

private:
  uint32_t value;
};

struct ClosureValue : Value {
  ClosureValue(const std::vector<Identifier> &args,
               const std::shared_ptr<Ast> &body,
               const std::map<Identifier, std::shared_ptr<Value>> environment);
  void accept(ValueVisitor &) const override;
  const std::vector<Identifier> &get_args() const;
  const std::shared_ptr<Ast> &get_body() const;
  std::shared_ptr<Value> apply(std::shared_ptr<Value> arg,
                               EvalVisitor &eval) const;

private:
  std::map<Identifier, std::shared_ptr<Value>> environment;
  const std::vector<Identifier> args;
  const std::shared_ptr<Ast> body;
};

struct ValueVisitor {
  virtual void visitNumber(const NumberValue &) = 0;
  virtual void visitClosure(const ClosureValue &) = 0;
};

struct EvalVisitor : Visitor {
  EvalVisitor();
  EvalVisitor(std::map<Identifier, std::shared_ptr<Value>>);
  void visitAssignment(const Assignment &let);
  void visitFn(const Fn &fn);
  void visitIfCond(const IfCond &if_cond);
  void visitApp(const App &app);
  void visitBinop(const Binop &op);
  void visitNumber(const Number &n);
  void visitIdentifier(const Identifier &id);
  void visitStatementExpr(const StatementExpr &statements);

  std::shared_ptr<Value> get_last(void) const;
  std::map<Identifier, std::shared_ptr<Value>> get_environment(void);
  void set_environment(std::map<Identifier, std::shared_ptr<Value>>);

private:
  std::map<Identifier, std::shared_ptr<Value>> environment;
  std::shared_ptr<Value> last;
};
