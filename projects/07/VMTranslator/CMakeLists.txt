cmake_minimum_required(VERSION 3.5)
project(VMTranslator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

add_executable(VMTranslator main.cpp parser.cpp generator.cpp )
