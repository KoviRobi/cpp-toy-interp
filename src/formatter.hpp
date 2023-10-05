#pragma once

#include "ast.hpp"

struct FmtVisitor : Visitor {
  void visitAssignment(const Assignment &let);
  void visitFn(const Fn &fn);
  void visitApp(const App &app);
  void visitBinop(const Binop &op);
  void visitNumber(const Number &n);
  void visitIdentifier(const Identifier &id);

  const std::string &operator*(void) const;
  void insert_semicolon(void);

private:
  std::string str = "";
};
