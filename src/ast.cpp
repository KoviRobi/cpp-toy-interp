#include "ast.hpp"

Assignment::Assignment(std::unique_ptr<Identifier> name,
                       std::unique_ptr<Expression> body)
    : name(std::move(name)), body(std::move(body)) {}
void Assignment::accept(Visitor &v) const { v.visitAssignment(*this); }
const Identifier &Assignment::get_name(void) const { return *name; }
const Expression &Assignment::get_body(void) const { return *body; }

Identifier::Identifier(std::string name) : name(name) {}
void Identifier::accept(Visitor &v) const { v.visitIdentifier(*this); }
const std::string &Identifier::operator*() const { return name; }
bool Identifier::operator<(const Identifier &other) const {
  return this->name < other.name;
}

Fn::Fn(std::vector<Identifier> args, std::vector<std::shared_ptr<Ast>> body)
    : args(std::move(args)), body(std::move(body)) {}
void Fn::accept(Visitor &v) const { v.visitFn(*this); }
const std::vector<Identifier> &Fn::get_args(void) const { return args; }
const std::vector<std::shared_ptr<Ast>> &Fn::get_body(void) const {
  return body;
}

App::App(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
    : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
void App::accept(Visitor &v) const { v.visitApp(*this); }
const Expression &App::get_lhs(void) const { return *lhs; }
const Expression &App::get_rhs(void) const { return *rhs; }

Binop::Binop(std::string op, std::unique_ptr<Expression> lhs,
             std::unique_ptr<Expression> rhs)
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
void Binop::accept(Visitor &v) const { v.visitBinop(*this); }
const std::string &Binop::get_op(void) const { return op; }
const Expression &Binop::get_lhs(void) const { return *lhs; }
const Expression &Binop::get_rhs(void) const { return *rhs; }

Number::Number(uint32_t value) : value(value) {}
void Number::accept(Visitor &v) const { v.visitNumber(*this); }
uint32_t Number::operator*() const { return value; }
