#include "lua_vm.h"

// void luaVm::getTabUp(Instruction inst){
//     vector<int> abc = inst.ABC();
//     abc[0] ++;
//     state.PushGlobalTable();
//     GetRK(abc[2]);
//     state.GetTable(-2);
//     state.Replace(abc[0]);
//     state.Pop(1);
// }

void luaVm::getUpvalue(Instruction inst){
    /// R(a) = UpValue(b)
    vector<int> abc = inst.ABC();
    abc[0]++;
    abc[1]++;
    state.Copy(state.LuaUpvalueIndex(abc[1]), abc[0]);
}

void luaVm::setUpvalue(Instruction inst){
    // Upvalue(b) = R(a)
    vector<int> abc = inst.ABC();
    abc[0]++;
    abc[1]++;
    state.Copy(abc[0], state.LuaUpvalueIndex(abc[1]));
}

void luaVm::getTabUp(Instruction inst){
    // R[a] = Upvalue[B][RK(C)]
    // combination of getUpval and getTable, but more efficiency
    vector<int> abc = inst.ABC();
    abc[0] ++;
    abc[1] ++;
    GetRK(abc[2]);
    state.GetTable(state.LuaUpvalueIndex(abc[1]));
    state.Replace(abc[0]);
}

void luaVm::setTabUp(Instruction inst){
    // Upvalue[a][RK[b]] = RK[c]
    vector<int> abc = inst.ABC();
    abc[0]++;
    GetRK(abc[1]);
    GetRK(abc[2]);
    state.SetTable(state.LuaUpvalueIndex(abc[0]));
}