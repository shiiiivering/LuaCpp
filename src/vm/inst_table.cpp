#include "lua_vm.h"
#include "fpb.h"
#include "common/lua.h"

void luaVm::newTable(Instruction inst){
    vector<int> abc = inst.ABC();
    abc[0] += 1;
    state.CreateTable(Fb2int(abc[1]), Int2fb(abc[2]));
    state.Replace(abc[0]);
}

void luaVm::getTable(Instruction inst){
    // R[a] = R[b][Rk[c]]
    vector<int> abc = inst.ABC();
    abc[0] ++;
    abc[1] ++;
    GetRK(abc[2]);
    state.GetTable(abc[1]);
    state.Replace(abc[0]);
}

void luaVm::setTable(Instruction inst){
    // R[a][RK(b)] = RK(c)
    vector<int> abc = inst.ABC();
    abc[0] ++;
    GetRK(abc[1]);
    GetRK(abc[2]);
    state.SetTable(abc[0]);
}

void luaVm::setList(Instruction inst){
    // R(a)[(c - 1) * FPF + i] = R(a + i), 1 <= i <= b
    vector<int> abc = inst.ABC();
    abc[0] ++;
    if(abc[2] > 0){
        abc[2] -= 1;
    }else{
        abc[2] = Fetch().Ax();
    }
    bool bZero = abc[1] == 0;
    if(bZero){
        abc[1] = state.ToInteger(-1) - abc[0] - 1;
        state.Pop(1);
    }
    lua_Integer idx = abc[2] * LFIELD_PER_FLUSH;
    for(int i = 1; i <= abc[1]; i++){
        idx++;
        state.PushValue(abc[0] + i);
        state.SetI(abc[0], idx);
    }
    if(bZero){
        for(int j = RegisterCount() + 1; j <= state.GetTop(); j++){ //need to compute each time?
            idx ++;
            state.PushValue(j);
            state.SetI(abc[0], idx);
        }

        state.SetTop(RegisterCount()); // clear stack
    }
}