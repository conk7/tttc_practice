cmake_minimum_required(VERSION 3.20)

project(Lab2LoopWrapper)

find_package(LLVM REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
include(AddLLVM)
add_llvm_library(Lab2LoopWrapper 
        MODULE plugin.cpp
        PLUGIN_TOOL opt
)