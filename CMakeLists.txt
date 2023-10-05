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
  src/main.cpp
)
