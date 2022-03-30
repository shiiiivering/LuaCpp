add_library(common STATIC
    lua_object.cpp
)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/common
)