#pragma once

/** \file
 * \brief Wraps readline for line-editing and history capabilities.
 */

#include <optional>
#include <string>

#define HISTORY_FILE ".tiny-interp-history"

struct Readline {
  Readline();
  std::optional<std::string> read(const std::string &prompt);

private:
  char *buffer;
};
