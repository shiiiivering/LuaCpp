add_library(state STATIC
    lua_stack.cpp
    lua_state.cpp
    lua_table.cpp
    lua_closure.cpp
    api_arith.cpp
    api_compare.cpp
    api_misc.cpp
    api_push.cpp
    api_get.cpp
    api_call.cpp
)
include_directories(
    ${PROJECT_SOURCE_DIR}/include/state
    ${PROJECT_SOURCE_DIR}/include
)