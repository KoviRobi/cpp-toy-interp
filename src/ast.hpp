#pragma once

/** \file
 * \brief The abstract syntax-tree of the language
 */

#include <cstdint>
#include <memory>
#include <vector>

struct Ast;
struct Assignment;
struct Expression;
struct Identifier;
struct Fn;
struct App;
struct Binop;
struct Number;
struct StatementExpr;

struct Visitor {
  virtual void visitAssignment(const Assignment &) = 0;
  virtual void visitFn(const Fn &) = 0;
  virtual void visitApp(const App &) = 0;
  virtual void visitBinop(const Binop &) = 0;
  virtual void visitNumber(const Number &) = 0;
  virtual void visitIdentifier(const Identifier &) = 0;
  virtual void visitStatementExpr(const StatementExpr &) = 0;
};

struct Ast {
  virtual void accept(Visitor &) const = 0;
  virtual ~Ast() = default;
};

struct Assignment : Ast {
  Assignment(std::unique_ptr<Identifier> name,
             std::unique_ptr<Expression> body);
  void accept(Visitor &) const override;
  const Identifier &get_name(void) const;
  const Expression &get_body(void) const;

private:
  const std::unique_ptr<Identifier> name;
  const std::unique_ptr<Expression> body;
};

struct Expression : Ast {};

struct Identifier : Expression {
  Identifier(std::string name);
  void accept(Visitor &) const override;
  const std::string &operator*() const;
  bool operator<(const Identifier &other) const;

private:
  const std::string name;
};

struct Fn : Expression {
  Fn(std::vector<Identifier> args, std::shared_ptr<Ast> body);
  void accept(Visitor &) const override;
  const std::vector<Identifier> &get_args(void) const;
  const std::shared_ptr<Ast> &get_body(void) const;

private:
  const std::vector<Identifier> args;
  const std::shared_ptr<Ast> body;
};

struct App : Expression {
  App(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);
  void accept(Visitor &) const override;
  const Expression &get_lhs(void) const;
  const Expression &get_rhs(void) const;

private:
  const std::unique_ptr<Expression> lhs;
  const std::unique_ptr<Expression> rhs;
};

struct Binop : Expression {
  Binop(std::string op, std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs);
  void accept(Visitor &) const override;
  const std::string &get_op(void) const;
  const Expression &get_lhs(void) const;
  const Expression &get_rhs(void) const;

private:
  const std::string op;
  const std::unique_ptr<Expression> lhs;
  const std::unique_ptr<Expression> rhs;
};

struct Number : Expression {
  Number(uint32_t value);
  void accept(Visitor &) const override;
  uint32_t operator*() const;

private:
  uint32_t value;
};

struct StatementExpr : Expression {
  StatementExpr(std::vector<std::unique_ptr<Ast>> body);
  void accept(Visitor &) const override;
  const std::vector<std::unique_ptr<Ast>> &get_body(void) const;

private:
  const std::vector<std::unique_ptr<Ast>> body;
};
