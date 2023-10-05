#include "formatter.hpp"
#include "parser.hpp"

#include <iomanip>
#include <iostream>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test parsing", "[parse]") {
  auto fmt_visitor = FmtVisitor();

  SECTION("Let-expression") {
    fmt_visitor.clear();
    for (auto &node : parse("let x = 1")) {
      node->accept(fmt_visitor);
      fmt_visitor.insert_semicolon();
    }
    REQUIRE("let x = 1 ; " == *fmt_visitor);
  }

  SECTION("Binary operator") {
    fmt_visitor.clear();
    for (auto &node : parse("1 + 2")) {
      node->accept(fmt_visitor);
      fmt_visitor.insert_semicolon();
    }
    REQUIRE("( 1 ) + ( 2 ) ; " == *fmt_visitor);
  }

  SECTION("Parentheses") {
    fmt_visitor.clear();
    for (auto &node : parse("( 1 + 2 )")) {
      node->accept(fmt_visitor);
      fmt_visitor.insert_semicolon();
    }
    REQUIRE("( 1 ) + ( 2 ) ; " == *fmt_visitor);
  }

  SECTION("Multiple binary operators") {
    fmt_visitor.clear();
    for (auto &node : parse("1 - 2 - ( 3 + 4 )")) {
      node->accept(fmt_visitor);
      fmt_visitor.insert_semicolon();
    }

    REQUIRE("( ( 1 ) - ( 2 ) ) - ( ( 3 ) + ( 4 ) ) ; " == *fmt_visitor);
  }

  SECTION("Functions") {
    fmt_visitor.clear();
    for (auto &node : parse("fn x { 1 }")) {
      node->accept(fmt_visitor);
      fmt_visitor.insert_semicolon();
    }
    REQUIRE("fn x { 1 } ; " == *fmt_visitor);
  }

  SECTION("Statements") {
    fmt_visitor.clear();
    for (auto &node : parse("let y = 1 ; fn x { let y = 2 ; y }")) {
      node->accept(fmt_visitor);
      fmt_visitor.insert_semicolon();
    }
    REQUIRE("let y = 1 ; fn x { let y = 2 ; y } ; " == *fmt_visitor);
  }
}
