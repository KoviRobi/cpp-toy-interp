#include "ast.hpp"

void Assignment::accept(Visitor &v) { v.visitAssignment(*this); }

NumExpr::NumExpr(uint32_t value) : value(value) {}
void NumExpr::accept(Visitor &v) { v.visitNumExpr(*this); }
uint32_t NumExpr::get() const { return this->value; }
