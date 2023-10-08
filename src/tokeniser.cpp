#include "tokeniser.hpp"

#include <iostream>

Tokeniser::Tokeniser(const std::string &str)
    : str(str), pos(str.cbegin()), eof(false) {}

pos_t Tokeniser::get_pos(void) const {
  return std::make_pair(this->pos, this->eof);
}
void Tokeniser::set_pos(pos_t newpos) {
  this->pos = newpos.first;
  this->eof = newpos.second;
}

void Tokeniser::print_cursor(void) const {
  std::cout << std::string_view(this->str.cbegin(), this->pos) << "|"
            << std::string_view(this->pos, this->str.cend()) << std::endl;
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

const std::string_view Tokeniser::rest() {
  return std::string_view(this->pos, this->str.cend());
}

bool Tokeniser::is_finished(void) { return eof; }
