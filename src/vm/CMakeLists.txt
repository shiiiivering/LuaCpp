add_library(vm STATIC  
    opcodes.cpp
    inst_for.cpp
    inst_load.cpp
    inst_misc.cpp
    inst_operations.cpp
    inst_table.cpp
    inst_call.cpp
    inst_upvalue.cpp
    fpb.cpp
    lua_vm.cpp
)
include_directories(
    ${PROJECT_SOURCE_DIR}/include/vm
    ${PROJECT_SOURCE_DIR}/include
)