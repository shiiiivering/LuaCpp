#include "lua_vm.h"
#include "instruction.h"
#include <vector>
using namespace std;

void luaVm::forPrep(Instruction inst){
    vector<int> asbx = inst.AsBx();
    asbx[0] += 1;

    // R(a) -= R(a + 2)
    state.PushValue(asbx[0]);
    state.PushValue(asbx[0] + 2);
    state.Arith(LUA_OPSUB);
    state.Replace(asbx[0]);
    // PC += sBx
    AddPC(asbx[1]);
}

void luaVm::forLoop(Instruction inst){
    vector<int> asbx = inst.AsBx();
    asbx[0] += 1;
    // R(a) += R(a + 2)
    state.PushValue(asbx[0] + 2);
    state.PushValue(asbx[0]);
    state.Arith(LUA_OPADD);
    state.Replace(asbx[0]);

    // R(a) <?= R(a + 1)
    // <?= means index(R(a)) not on or beyond the limit(R(a + 2))
    bool positiveStep = state.ToNumber(asbx[0] + 2) >= 0;
    if((positiveStep && state.Compare(asbx[0], asbx[0] + 1, LUA_OPLE)) 
        || (!positiveStep && state.Compare(asbx[0] + 1, asbx[0], LUA_OPLE))){
            AddPC(asbx[1]);
            state.Copy(asbx[0], asbx[0] + 3);        
    }
}

void luaVm::tForLoop(Instruction inst){
    vector<int> asbx = inst.AsBx();
    asbx[0] ++;
    if(!state.IsNil(asbx[0] + 1)){
        state.Copy(asbx[0] + 1, asbx[0]);
        AddPC(asbx[1]);
    }
}