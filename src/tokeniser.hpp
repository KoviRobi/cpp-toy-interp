#pragma once

/** \file
 * \brief An overly simple tokeniser, only space based so you need to use e.g.
 * `( 1 + 2 )` instead of `(1+2)`
 */

#include <exception>
#include <optional>
#include <string>

struct ParseError : public std::exception {};

struct EofToken : public ParseError {};

typedef std::pair<std::string::const_iterator, bool> pos_t;

struct Tokeniser {
  Tokeniser(const std::string &str);

  pos_t get_pos(void) const;
  void set_pos(pos_t);

  void print_cursor(void) const;

  const std::string_view next_token();

  const std::string_view rest();

  bool is_finished(void);

private:
  const std::string &str;
  std::string::const_iterator pos;
  bool eof;
};
