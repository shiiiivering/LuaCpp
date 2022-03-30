include_directories(
    ${PROJECT_SOURCE_DIR}/include/number
    ${PROJECT_SOURCE_DIR}/include
)
add_library(
    number STATIC
    lua_math.cpp
)
