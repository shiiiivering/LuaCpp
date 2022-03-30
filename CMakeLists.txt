cmake_minimum_required (VERSION 3.21)

project(LuaCpp VERSION 0.1)

# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g)

add_subdirectory(src)

add_executable(LuaCpp src/main.cpp)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(LuaCpp vm state common number binaryChunk)