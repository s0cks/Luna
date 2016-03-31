cmake_minimum_required(VERSION 3.3)
project(Luna)

include_directories(Sources)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(Luna main.cc Sources/type.h Sources/type.cc Sources/global.h Sources/assembler.cc Sources/assembler.h Sources/assembler_x64.h Sources/constants_x64.h Sources/assembler_x64.cc Sources/intermediate_language.h Sources/intermediate_language_x64.cc Sources/parser.h Sources/parser.cc Sources/compiler.cc Sources/compiler.h Sources/vm.cc Sources/vm.h Sources/array.h Sources/ast.h Sources/scope.h Sources/ast.cc Sources/stack_frame.h Sources/stack_frame_x64.h Sources/os.h Sources/os_linux.cc)