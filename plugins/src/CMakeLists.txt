cmake_minimum_required(VERSION 3.20)

project(ImplicitCastsCounter)

# для удобства добавил сбилженные файлы в path
find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

include(AddLLVM)

add_llvm_library(ImplicitCastsCounter 
        MODULE ImplicitCastsCounter.cpp 
        PLUGIN_TOOL clang)
