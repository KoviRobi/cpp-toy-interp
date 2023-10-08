#include "formatter.hpp"

#include <iostream>

void formatFn(const std::vector<Identifier> &args,
              const std::shared_ptr<Ast> &body,
              std::function<void(const std::string &)> output,
              FmtAst &visitor) {
  output("fn ");

  bool first = true;
  for (auto &arg : args) {
    if (!first)
      output(" , ");
    first = false;
    arg.accept(visitor);
  }

  output(" ");

  body->accept(visitor);
}

// AST formatter

FmtAst::FmtAst(std::function<void(const std::string &)> output)
    : output(output) {}

void FmtAst::visitAssignment(const Assignment &let) {
  output("let ");
  output(*let.get_name());
  output(" = ");
  let.get_body().accept(*this);
}

void FmtAst::visitFn(const Fn &fn) {
  formatFn(fn.get_args(), fn.get_body(), output, *this);
};

void FmtAst::visitApp(const App &app) {
  output("( ");
  app.get_lhs().accept(*this);
  output(" ) ( ");
  app.get_rhs().accept(*this);
  output(" )");
};

void FmtAst::visitBinop(const Binop &op) {
  output("( ");
  op.get_lhs().accept(*this);
  output(" ) " + op.get_op() + " ( ");
  op.get_rhs().accept(*this);
  output(" )");
};

void FmtAst::visitNumber(const Number &n) { output(std::to_string(*n)); }

void FmtAst::visitIdentifier(const Identifier &id) { output(*id); };

void FmtAst::visitStatementExpr(const StatementExpr &statements) {
  output("{ ");
  bool first = true;
  for (auto &statement : statements.get_body()) {
    if (!first)
      output(" ; ");
    first = false;
    statement->accept(*this);
  }
  output(" }");
};

// Value formatter

FmtValue::FmtValue(FmtAst &ast_visitor,
                   std::function<void(const std::string &)> output)
    : ast_visitor(ast_visitor), output(output) {}

void FmtValue::visitNumber(const NumberValue &n) {
  output(std::to_string(n.get_value()));
}

void FmtValue::visitClosure(const ClosureValue &c) {
  formatFn(c.get_args(), c.get_body(), output, ast_visitor);
}
