#include "lua_vm.h"
#include "instruction.h"
#include <vector>
using namespace std;

void luaVm::_binaryArith(Instruction inst, ArithOp op){
    // R[a] = RK[b] op RK[c]
    vector<int> abc = inst.ABC();
    abc[0] += 1;
    GetRK(abc[1]);
    GetRK(abc[2]);
    state.Arith(op);
    state.Replace(abc[0]);
}

void luaVm::_unaryArith(Instruction inst, ArithOp op){
    // R[a] = op R[b]
    vector<int> abc = inst.ABC();
    abc[0] += 1;
    abc[1] += 1;
    state.PushValue(abc[1]);
    state.Arith(op);
    state.Replace(abc[0]);
}

void luaVm::_len(Instruction inst){
    vector<int> abc = inst.ABC();
    abc[0] ++;
    abc[1] ++;
    state.Len(abc[1]);
    state.Replace(abc[0]);
}

void luaVm::concat(Instruction inst){
    // load values from stack[b] to stack[c] and concat them
    // store the result to stack[a]
    vector<int> abc = inst.ABC();
    for(int & i : abc) i++;
    int n = abc[2] - abc[1] + 1;
    for(int i = abc[1]; i <= abc[2]; i++){
        state.PushValue(i);
    }
    state.Concat(n);
    state.Replace(abc[0]);
}

void luaVm::_compare(Instruction inst, CompareOp op){
    // if((RK(B) op RK(C)) != A) then pc++
    vector<int> abc = inst.ABC();

    GetRK(abc[1]);
    GetRK(abc[2]);
    if(state.Compare(-2, -1, op) != (abc[0] != 0)){
        AddPC(1);
    }
    state.Pop(2);
}

void luaVm::vm_not(Instruction inst){
    // R[a] = not R[b]
    vector<int> abc = inst.ABC();
    abc[0] ++;
    abc[1] ++;
    state.PushBoolean(!state.ToBoolean(abc[1]));
    state.Replace(abc[0]);
}

void luaVm::testSet(Instruction inst){
    // if(R(b) <=> C) then R(a) = R(b) else PC++
    // <=> means equal in bool value
    vector<int> abc = inst.ABC();
    abc[0] ++;
    abc[1] ++;
    if(state.ToBoolean(abc[1]) == (abc[2] != 0)){
        state.Copy(abc[1], abc[0]);
    }else{
        AddPC(1);
    }
}

void luaVm::test(Instruction inst){
    // if not (R(a) <=> C) then PC++
    // do not change value in stack
    vector<int> abc = inst.ABC();
    abc[0] ++;
    if(state.ToBoolean(abc[0]) != (abc[2] != 0)){
        AddPC(1);
    }
}