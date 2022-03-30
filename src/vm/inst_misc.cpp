#include "lua_vm.h"
#include "instruction.h"
#include <vector>
using namespace std;

void luaVm::move(Instruction inst){
    vector<int> abc = inst.ABC();
    abc[0] += 1;
    abc[1] += 1;
    state.Copy(abc[1], abc[0]);
}

void luaVm::jmp(Instruction inst){
    // pc += sbx; if(a != 0) then:  
    //      {close all upvalues >= R(a - 1)}
    vector<int> asbx = inst.AsBx();
    AddPC(asbx[1]);
    if (asbx[0] != 0){
        // when some local values in a block captured by a
        // nested function and the values got out of their
        // action scope(block over), compiler will generate
        // a JMP instruction and tell the luaVm to close the 
        // relative Upvalue
        CloseUpvalues(asbx[0]);
    }
}

