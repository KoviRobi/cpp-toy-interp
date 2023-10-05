#include "parser.hpp"
#include "tokeniser.hpp"

#include <algorithm>
#include <string>
#include <vector>

/**
 * Parses the following grammar:
 *
 *     root -> statements.
 *
 *     statements -> statement (";" statement)*.
 *
 *     statement -> "let" id "=" expr.
 *     statement -> expr.
 *
 *     vars -> id ("," id)*.
 *
 *     expr -> "fn" vars "{" statements "}".
 *     expr -> app.
 *
 *     app -> infix app?.
 *
 *     infix -> term (binop term)*.
 *
 *     binop -> "<" | "==" | ">" | "+" | "-".
 *
 *     term -> "(" expr ")"
 *     term -> number
 *     term -> id
 */
struct Parser {
  struct BadToken : public ParseError {
    BadToken(const std::string str) : str(str) {}

    const char *what() const noexcept override { return str.c_str(); }

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
  std::vector<Identifier> vars(void);
  std::unique_ptr<Expression> expr(void);
  std::unique_ptr<Ast> statement(void);
  std::vector<std::unique_ptr<Ast>> statements(void);

private:
  std::unique_ptr<Tokeniser> tokr;
};

Parser::Parser(const std::string &str)
    : tokr(std::make_unique<Tokeniser>(str)) {}

bool Parser::is_id_cont(const std::string_view &s) {
  return s.size() == 0 || std::all_of(s.begin(), s.end(), [](unsigned char c) {
           return std::isalnum(c) || c == '_';
         });
}

bool Parser::is_id(const std::string_view &s) {
  return s.size() > 0 && (std::isalpha((unsigned char)s[0]) || s == "_") &&
         is_id_cont(s.substr(1));
}

bool Parser::is_num(const std::string_view &s) {
  return std::all_of(s.begin(), s.end(),
                     [](unsigned char c) { return std::isdigit(c); });
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

std::unique_ptr<Expression> Parser::infix(void) {
  auto ast = term();
  auto pos = tokr->get_pos();
  std::string_view tok;
  try {
    tok = tokr->next_token();
    while (is_binop(tok)) {
      auto rhs = term();
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

std::unique_ptr<Expression> Parser::app(void) {
  auto ast = infix();
  auto pos = tokr->get_pos();
  try {
    auto rhs = app();
    ast = std::make_unique<App>(std::move(ast), std::move(rhs));
  } catch (const ParseError &e) {
    tokr->set_pos(pos);
  }
  return ast;
}

std::vector<Identifier> Parser::vars(void) {
  std::vector<Identifier> ret;
  auto pos = tokr->get_pos();
  std::string_view tok = tokr->next_token();
  while (is_id(tok)) {
    ret.push_back(Identifier(std::string(tok)));
    pos = tokr->get_pos();
    tok = tokr->next_token();
  }
  tokr->set_pos(pos);
  return ret;
}

std::unique_ptr<Expression> Parser::expr(void) {
  auto pos = tokr->get_pos();
  std::string_view tok = tokr->next_token();

  if (tok == "fn") {
    auto args = vars();
    expect("{");
    auto body = statements();
    expect("}");
    return std::make_unique<Fn>(std::move(args), std::move(body));
  }

  tokr->set_pos(pos);
  return app();
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
  ret.push_back(statement());
  auto pos = tokr->get_pos();
  std::string_view tok;
  try {
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

std::vector<std::unique_ptr<Ast>> parse(const std::string &str) {
  auto parser = Parser(str);
  return parser.statements();
}
