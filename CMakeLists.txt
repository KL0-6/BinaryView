cmake_minimum_required(VERSION 3.8)

project(BinaryView)

add_executable(BinaryView 
    main.cpp
)

set_target_properties(BinaryView PROPERTIES CXX_STANDARD 17)