cmake_minimum_required(VERSION 3.15)

project(
  "tiny-interp"
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tiny-interp
  src/tokeniser.cpp
  src/ast.cpp
  src/parser.cpp
  src/formatter.cpp
  src/main.cpp
  src/readline.cpp
)

find_package(Catch2 3 REQUIRED)

add_executable(test_parser
  test/test_parser.cpp
  src/tokeniser.cpp
  src/ast.cpp
  src/parser.cpp
  src/formatter.cpp
)
target_include_directories(test_parser PRIVATE ./src)
target_link_libraries(test_parser PRIVATE Catch2::Catch2WithMain)
