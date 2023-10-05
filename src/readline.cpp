#include "readline.hpp"

extern "C" {
#include <readline/history.h>
#include <readline/readline.h>
}

Readline::Readline() : buffer(nullptr) {
  using_history();
  read_history(HISTORY_FILE);
}

std::optional<std::string> Readline::read(const std::string &prompt) {
  if (buffer)
    std::free(buffer);
  buffer = readline(prompt.c_str());
  if (buffer) {
    add_history(buffer);
    write_history(HISTORY_FILE);
    return std::string(buffer);
  } else {
    return std::nullopt;
  }
}
