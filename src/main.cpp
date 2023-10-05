#include "tokeniser.hpp"

#include <iostream>
#include <optional>

int main(int argc, char *argv[]) {
  const std::string hello = "Hello, world";
  auto tokeniser = Tokeniser(hello);
  std::optional<std::string> tok;
  while ((tok = tokeniser.next_token())) {
    std::cout << "Token: \"" << *tok << "\"" << std::endl;
  }
}
