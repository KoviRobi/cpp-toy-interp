#pragma once

#include "ast.hpp"

#include <map>
#include <vector>

struct EvalError : std::exception {};

struct NotANumber : EvalError {};
struct NotAFunction : EvalError {};
struct UnknownVariable : EvalError {};

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
               const std::vector<std::shared_ptr<Ast>> &body);
  void accept(ValueVisitor &) const override;
  const std::vector<Identifier> &get_args() const;
  const std::vector<std::shared_ptr<Ast>> &get_body() const;
  std::shared_ptr<Value> apply(std::shared_ptr<Value> arg,
                               EvalVisitor &eval) const;

private:
  std::map<Identifier, std::shared_ptr<Value>> environment;
  const std::vector<Identifier> &args;
  const std::vector<std::shared_ptr<Ast>> &body;
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
  void visitApp(const App &app);
  void visitBinop(const Binop &op);
  void visitNumber(const Number &n);
  void visitIdentifier(const Identifier &id);

  std::shared_ptr<Value> get_last(void) const;

private:
  std::map<Identifier, std::shared_ptr<Value>> environment;
  std::shared_ptr<Value> last;
};
