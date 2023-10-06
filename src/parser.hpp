#pragma once

/** \file
 * \brief Parses the following grammar:
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

#include "ast.hpp"

#include <memory>
#include <string>
#include <vector>

std::vector<std::unique_ptr<Ast>> parse(const std::string &str);
