#pragma once

#include "ast.hpp"

#include <memory>
#include <string>
#include <vector>

std::vector<std::unique_ptr<Ast>> parse(const std::string &str);
