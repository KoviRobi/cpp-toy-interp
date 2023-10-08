#include "parser.hpp"
#include "tokeniser.hpp"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

struct Parser {
  struct BadToken : public ParseError {
    BadToken(const std::string str) : str(str) {}

  private:
    std::string str;
  };

  struct LeftoverString : public ParseError {
    LeftoverString(const std::string str) : str(str) {}

  private:
    std::string str;
  };

  Parser(const std::string &str);

  bool is_id_cont(const std::string_view &s);
  bool is_id(const std::string_view &s);
  bool is_num(const std::string_view &s);
  void expect(std::string s);
  std::unique_ptr<Expression> term(void);
  static bool is_binop(std::string_view tok);
  std::unique_ptr<Expression> infix(void);
  std::unique_ptr<Expression> app(void);
  Identifier id(void);
  std::vector<Identifier> vars(void);
  std::unique_ptr<Expression> expr(void);
  std::unique_ptr<Ast> statement(void);
  std::vector<std::unique_ptr<Ast>> statements(void);

  void assert_finished(void);

private:
  std::set<std::string> builtins;
  std::unique_ptr<Tokeniser> tokr;
};

Parser::Parser(const std::string &str)
    : tokr(std::make_unique<Tokeniser>(str)),
      builtins({"let", "fn", "if", "then", "else"}) {}

bool Parser::is_id_cont(const std::string_view &s) {
  return s.size() == 0 || std::all_of(s.begin(), s.end(), [](unsigned char c) {
           return std::isalnum(c) || c == '_';
         });
}

bool Parser::is_id(const std::string_view &s) {
  return s.size() > 0 && (std::isalpha((unsigned char)s[0]) || s == "_") &&
         is_id_cont(s.substr(1)) && !builtins.contains(std::string(s));
}

bool Parser::is_num(const std::string_view &s) {
  return s.size() > 0 && std::all_of(s.begin(), s.end(), [](unsigned char c) {
           return std::isdigit(c);
         });
}

void Parser::expect(std::string s) {
  if (tokr->next_token() != s) {
    throw BadToken(s);
  }
}

std::unique_ptr<Expression> Parser::term(void) {
  const std::string_view tok = tokr->next_token();

  if (tok == "(") {
    auto ast = expr();
    expect(")");
    return ast;
  }

  if (tok == "{") {
    auto ast = statements();
    expect("}");
    return std::make_unique<StatementExpr>(std::move(ast));
  }

  if (is_num(tok)) {
    auto i = std::stoi(std::string(tok));
    return std::make_unique<Number>(i);
  }

  if (is_id(tok)) {
    return std::make_unique<Identifier>(std::string(tok));
  }

  throw BadToken(std::string(tok));
}

bool Parser::is_binop(std::string_view tok) {
  return tok == "<" || tok == "==" || tok == ">" || tok == "+" || tok == "-";
}

std::unique_ptr<Expression> Parser::app(void) {
  auto ast = term();
  auto pos = tokr->get_pos();
  try {
    auto rhs = app();
    ast = std::make_unique<App>(std::move(ast), std::move(rhs));
  } catch (const ParseError &e) {
    tokr->set_pos(pos);
  }
  return ast;
}

std::unique_ptr<Expression> Parser::infix(void) {
  auto ast = app();
  auto pos = tokr->get_pos();
  std::string_view tok;
  try {
    tok = tokr->next_token();
    while (is_binop(tok)) {
      auto rhs = app();
      ast = std::make_unique<Binop>(std::string(tok), std::move(ast),
                                    std::move(rhs));
      pos = tokr->get_pos();
      tok = tokr->next_token();
    }
  } catch (const ParseError &e) {
  }
  // Deliberately after the catch -- we want to set it back if the loop
  // terminates
  tokr->set_pos(pos);
  return ast;
}

Identifier Parser::id(void) {
  std::string_view tok = tokr->next_token();
  if (!is_id(tok))
    throw BadToken(std::string(tok));
  return Identifier(std::string(tok));
}

std::vector<Identifier> Parser::vars(void) {
  std::vector<Identifier> ret;
  bool parenthesised = false;
  auto pos = tokr->get_pos();
  std::string_view tok = tokr->next_token();

  if (tok == "(") {
    parenthesised = true;
  } else {
    tokr->set_pos(pos);
  }

  ret.push_back(id());

  try {
    pos = tokr->get_pos();
    tok = tokr->next_token();
    while (tok == ",") {
      ret.push_back(id());
      pos = tokr->get_pos();
      tok = tokr->next_token();
    }
  } catch (const ParseError &e) {
  }
  // Deliberately after the catch -- we want to set it back if the loop
  // terminates
  tokr->set_pos(pos);

  if (parenthesised)
    expect(")");

  return ret;
}

std::unique_ptr<Expression> Parser::expr(void) {
  auto pos = tokr->get_pos();
  std::string_view tok = tokr->next_token();

  if (tok == "fn") {
    auto args = vars();
    // The body is going to be kept around for longer for e.g. closures
    auto body = expr();
    return std::make_unique<Fn>(std::move(args), std::move(body));
  }

  if (tok == "if") {
    auto condition = expr();
    expect("then");
    auto true_case = expr();
    expect("else");
    auto false_case = expr();
    return std::make_unique<IfCond>(std::move(condition), std::move(true_case),
                                    std::move(false_case));
  }

  tokr->set_pos(pos);
  return infix();
}

std::unique_ptr<Ast> Parser::statement(void) {
  auto pos = tokr->get_pos();
  std::string_view tok = tokr->next_token();

  if (tok == "let") {
    tok = tokr->next_token();
    if (!is_id(tok))
      throw BadToken(std::string(tok));
    auto id = std::make_unique<Identifier>(std::string(tok));
    expect("=");
    auto body = expr();
    return std::make_unique<Assignment>(std::move(id), std::move(body));
  }

  tokr->set_pos(pos);
  return expr();
}

std::vector<std::unique_ptr<Ast>> Parser::statements(void) {
  std::vector<std::unique_ptr<Ast>> ret;
  auto pos = tokr->get_pos();
  try {
    std::string_view tok;
    ret.push_back(statement());
    pos = tokr->get_pos();
    tok = tokr->next_token();
    while (tok == ";") {
      ret.push_back(statement());
      pos = tokr->get_pos();
      tok = tokr->next_token();
    }
  } catch (const ParseError &e) {
  }
  // Deliberately after the catch -- we want to set it back if the loop
  // terminates
  tokr->set_pos(pos);

  return ret;
}

void Parser::assert_finished(void) {
  if (tokr->is_finished())
    return;
  if (tokr->rest() == "")
    return;
  throw LeftoverString(std::string(tokr->rest()));
}

std::vector<std::unique_ptr<Ast>> parse(const std::string &str) {
  auto parser = Parser(str);
  auto ast = parser.statements();
  parser.assert_finished();
  return ast;
}
