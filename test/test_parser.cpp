#include "formatter.hpp"
#include "parser.hpp"
#include "tokeniser.hpp"

#include <iomanip>
#include <iostream>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test parsing", "[parse]") {
  std::string formatted;
  FmtAst ast_formatter([&](auto s) { formatted += s; });

  SECTION("Empty string") {
    formatted = "";
    for (auto &node : parse("")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("" == formatted);
  }

  SECTION("Malformatted string") {
    formatted = "";
    REQUIRE_THROWS_AS(parse("} unparsed"), ParseError);
    for (auto &node : parse("")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("" == formatted);
  }

  SECTION("Let-expression") {
    formatted = "";
    for (auto &node : parse("let x = 1")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("let x = 1 ; " == formatted);
  }

  SECTION("Binary operator") {
    formatted = "";
    for (auto &node : parse("1 + 2")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("( 1 ) + ( 2 ) ; " == formatted);
  }

  SECTION("Parentheses") {
    formatted = "";
    for (auto &node : parse("( 1 + 2 )")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("( 1 ) + ( 2 ) ; " == formatted);
  }

  SECTION("Multiple binary operators") {
    formatted = "";
    for (auto &node : parse("1 - 2 - ( 3 + 4 )")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }

    REQUIRE("( ( 1 ) - ( 2 ) ) - ( ( 3 ) + ( 4 ) ) ; " == formatted);
  }

  SECTION("Functions") {
    formatted = "";
    for (auto &node : parse("fn x { 1 }")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("fn x { 1 } ; " == formatted);
  }

  SECTION("Statements") {
    formatted = "";
    for (auto &node : parse("let y = 1 ; fn x { let y = 2 ; y }")) {
      node->accept(ast_formatter);
      formatted += " ; ";
    }
    REQUIRE("let y = 1 ; fn x { let y = 2 ; y } ; " == formatted);
  }
}
