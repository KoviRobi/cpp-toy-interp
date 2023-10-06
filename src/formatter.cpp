#include "formatter.hpp"

void FmtVisitor::visitAssignment(const Assignment &let) {
  str += "let " + *let.get_name() + " = ";
  let.get_body().accept(*this);
}

void FmtVisitor::visitFn(const Fn &fn) {
  str += "fn ";

  bool first = true;
  for (auto &arg : fn.get_args()) {
    if (!first)
      str += " , ";
    first = false;
    arg.accept(*this);
  }

  str += " { ";
  first = true;
  for (auto &body : fn.get_body()) {
    if (!first)
      insert_semicolon();
    first = false;
    body->accept(*this);
  }
  str += " }";
};

void FmtVisitor::visitApp(const App &app) {
  app.get_lhs().accept(*this);
  str += " ";
  app.get_rhs().accept(*this);
};

void FmtVisitor::visitBinop(const Binop &op) {
  str += "( ";
  op.get_lhs().accept(*this);
  str += " ) " + op.get_op() + " ( ";
  op.get_rhs().accept(*this);
  str += " )";
};

void FmtVisitor::visitNumber(const Number &n) { str += std::to_string(*n); }

void FmtVisitor::visitIdentifier(const Identifier &id) { str += *id; };

const std::string &FmtVisitor::operator*(void) const { return str; }

void FmtVisitor::insert_semicolon(void) { str += " ; "; }

void FmtVisitor::clear(void) { str = ""; }
