cmake_minimum_required(VERSION 3.15)

project(
  "tiny-interp"
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(tiny-interp-lib STATIC
  src/tokeniser.cpp
  src/ast.cpp
  src/parser.cpp
  src/formatter.cpp
  src/readline.cpp
  src/eval.cpp)
target_include_directories(tiny-interp-lib PUBLIC src)

add_executable(tiny-interp src/main.cpp)
target_link_libraries(tiny-interp tiny-interp-lib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(READLINE REQUIRED readline)

target_link_libraries(tiny-interp ${READLINE_LIBRARIES})
target_include_directories(tiny-interp PUBLIC ${READLINE_INCLUDE_DIRS})
target_compile_options(tiny-interp PUBLIC ${READLINE_CFLAGS_OTHER})

install(TARGETS tiny-interp)

find_package(Catch2 3 REQUIRED)

add_executable(tests
  test/tests.cpp
)
target_include_directories(tests PRIVATE ./src)
target_link_libraries(tests PUBLIC tiny-interp-lib PRIVATE Catch2::Catch2WithMain)
