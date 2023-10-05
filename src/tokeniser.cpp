#include "tokeniser.hpp"

#include <iostream>

std::string::const_iterator Tokeniser::getpos() const { return this->pos; }
void Tokeniser::setpos(std::string::const_iterator newpos) {
  this->pos = newpos;
}

const std::string_view Tokeniser::next_token() {
  if (eof)
    throw EofToken();

  std::string::const_iterator cursor = this->pos;

  while (cursor != this->str.cend() && std::isspace(*cursor)) {
    ++cursor;
  }
  this->pos = cursor;

  while (cursor != this->str.cend() && !std::isspace(*cursor)) {
    ++cursor;
  }

  if (cursor == this->str.cend()) {
    eof = true;
  }
  auto tok = std::string_view(this->pos, cursor);

  this->pos = cursor;
  return tok;
}
