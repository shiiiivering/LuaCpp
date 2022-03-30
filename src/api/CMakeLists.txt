add_library(api STATIC
    cpp_function.cpp
)
include_directories(
    ${PROJECT_SOURCE_DIR}/include/api
    ${PROJECT_SOURCE_DIR}/include/
)