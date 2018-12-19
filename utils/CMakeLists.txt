cmake_minimum_required(VERSION 3.13)
SET(CMAKE_CXX_STANDARD 14)

find_package(Clang REQUIRED)

message (INFO " using LLVM from: " ${LLVM_BUILD_BINARY_DIR})
message (INFO " using LLVM version: " ${LLVM_PACKAGE_VERSION})

list(APPEND CMAKE_MODULE_PATH ${LLVM_LIBRARY_DIRS}/cmake/llvm)
include (AddLLVM)

add_executable(jsongen jsongen.cpp)
llvm_update_compile_flags(jsongen)
add_link_opts(jsongen)
target_link_libraries(jsongen clangBasic clangAST clangTooling clangFrontend clangSerialization LLVM)
target_include_directories(jsongen SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
