#pragma once

#include <cstdint>

struct Ast;
struct Assignment;
struct Expression;
struct NumExpr;

struct Visitor {
  virtual void visitAssignment(Assignment &) = 0;
  virtual void visitNumExpr(NumExpr &) = 0;
};

struct Ast {
  virtual void accept(Visitor &) = 0;
  virtual ~Ast() = default;
};

struct Assignment : Ast {
  void accept(Visitor &) override;
};

struct Expression : Ast {};

struct NumExpr : Expression {
  NumExpr(uint32_t value);
  void accept(Visitor &) override;
  uint32_t get() const;

private:
  uint32_t value;
};
