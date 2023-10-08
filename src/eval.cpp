#include "eval.hpp"

#include <cassert>

NumberValue::NumberValue(uint32_t value) : value(value) {}
void NumberValue::accept(ValueVisitor &v) const { v.visitNumber(*this); }
uint32_t NumberValue::get_value() const { return value; }

ClosureValue::ClosureValue(
    const std::vector<Identifier> &args,
    const std::vector<std::shared_ptr<Ast>> &body,
    const std::map<Identifier, std::shared_ptr<Value>> environment)
    : environment(environment), args(args), body(body) {}

void ClosureValue::accept(ValueVisitor &v) const { v.visitClosure(*this); }

const std::vector<Identifier> &ClosureValue::get_args() const { return args; }

const std::vector<std::shared_ptr<Ast>> &ClosureValue::get_body() const {
  return body;
}

std::shared_ptr<Value> ClosureValue::apply(std::shared_ptr<Value> arg,
                                           EvalVisitor &eval) const {
  auto inner_environment(environment);
  auto formal_begin = args.cbegin();
  auto formal_end = args.cend();
  auto formal = *formal_begin;
  assert(formal_begin != formal_end);
  inner_environment[formal] = arg;

  if (args.size() == 1) {
    auto outer_environment = eval.get_environment();
    eval.set_environment(inner_environment);
    for (auto &node : body) {
      node->accept(eval);
    }
    auto result = eval.get_last();
    eval.set_environment(outer_environment);
    return result;
  } else {
    ++formal_begin;
    std::vector<Identifier> rest_args(formal_begin, formal_end);
    return std::make_shared<ClosureValue>(rest_args, body, inner_environment);
  }
}

EvalVisitor::EvalVisitor() : environment({}), last(nullptr) {}

EvalVisitor::EvalVisitor(
    std::map<Identifier, std::shared_ptr<Value>> other_environment)
    : environment(other_environment), last(nullptr) {}

void EvalVisitor::visitAssignment(const Assignment &let) {
  let.get_body().accept(*this);
  environment[*let.get_name()] = last;
}

void EvalVisitor::visitFn(const Fn &fn) {
  std::vector<Ast> body;
  last =
      std::make_shared<ClosureValue>(fn.get_args(), fn.get_body(), environment);
}

void EvalVisitor::visitApp(const App &app) {
  app.get_lhs().accept(*this);
  auto lhs = std::dynamic_pointer_cast<ClosureValue>(last);
  if (lhs == nullptr)
    throw NotAFunction();

  app.get_rhs().accept(*this);
  auto rhs = last;

  last = lhs->apply(rhs, *this);
}

void EvalVisitor::visitBinop(const Binop &op) {
  op.get_lhs().accept(*this);
  auto lhs = std::dynamic_pointer_cast<NumberValue>(last);
  if (lhs == nullptr)
    throw NotANumber();

  op.get_rhs().accept(*this);
  auto rhs = std::dynamic_pointer_cast<NumberValue>(last);
  if (rhs == nullptr)
    throw NotANumber();

  auto opname = op.get_op();
  if (opname == "<") {
    last = std::make_shared<NumberValue>(lhs->get_value() < rhs->get_value());
  }
  if (opname == "=") {
    last = std::make_shared<NumberValue>(lhs->get_value() == rhs->get_value());
  }
  if (opname == ">") {
    last = std::make_shared<NumberValue>(lhs->get_value() > rhs->get_value());
  }
  if (opname == "+") {
    last = std::make_shared<NumberValue>(lhs->get_value() + rhs->get_value());
  }
  if (opname == "-") {
    last = std::make_shared<NumberValue>(lhs->get_value() - rhs->get_value());
  }
}

void EvalVisitor::visitNumber(const Number &number) {
  last = std::make_unique<NumberValue>(*number);
}

void EvalVisitor::visitIdentifier(const Identifier &id) {
  if (environment.contains(id)) {
    last = environment[id];
  } else {
    throw UnknownVariable();
  }
}

std::shared_ptr<Value> EvalVisitor::get_last(void) const { return last; }

std::map<Identifier, std::shared_ptr<Value>>
EvalVisitor::get_environment(void) {
  return environment;
}
void EvalVisitor::set_environment(
    std::map<Identifier, std::shared_ptr<Value>> new_environment) {
  environment = new_environment;
}
