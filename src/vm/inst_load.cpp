#include "lua_vm.h"
#include "instruction.h"
#include <vector>
using namespace std;

void luaVm::loadNil(Instruction inst){
    // set b + 1 values to be Nil
    // stack[a], stack[a + 1] .... stack[a + b]
    vector<int> abc = inst.ABC();
    abc[0] += 1;
    state.PushNil(); 
    for(int i = abc[0]; i < abc[0] + abc[1]; i++){
        state.Copy(-1, i);
    }
    state.Pop(1);
}
void luaVm::loadBool(Instruction inst){
    // load B to A, if B == 0 then load false , else load true
    // If C is not 0, then skip the next instruction
    vector<int> abc = inst.ABC();
    abc[0] += 1;
    state.PushBoolean(abc[1] != 0);
    state.Replace(abc[0]);;
    if(abc[2]){
        AddPC(1);
    }
}

void luaVm::loadK(Instruction inst){
    // load constant[bx] to stack[a]
    // max bx is 262143. Cover most situation
    vector<int> abx = inst.ABx();
    abx[0] += 1;
    GetConst(abx[1]);
    state.Replace(abx[0]);
}

void luaVm::loadKx(Instruction inst){
    // load const[ax] to top stack, ax stores in the next 
    // instruction. In case const table is too large for bx 
    // to index
    vector<int> abx = inst.ABx();
    abx[0] = 1;
    int ax = Fetch().Ax();

    GetConst(ax);
    state.Replace(abx[0]);
}