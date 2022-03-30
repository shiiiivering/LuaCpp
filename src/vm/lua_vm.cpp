#include "state/lua_state.h"
#include "lua_vm.h"
#include "instruction.h"
int luaVm::PC(){
    return state.stack->pc;
}

void luaVm::AddPC(int n){
    state.stack->pc += n;
}

Instruction luaVm::Fetch(){ // return instruction or uint_32?
    Instruction ins = state.GetCurrentInstruction();
    AddPC(1);
    return ins;
}

void luaVm::GetConst(int idx){
    state.Push(state.GetConstant(idx));
}
void luaVm::GetRK(int rk){ // actually type OpArgK argument in iABC
    if(rk > 0xff){ // constant
        GetConst(rk & 0xff);
    }
    else{
        state.PushValue(rk + 1);
    }
}

void luaVm::Execute(Instruction inst){
    auto action = opcodes[inst.Opcode()].action;
    if(action != nullptr){
        (this->*action)(inst);
    }
    else{
        panic("TODO" + inst.GetOpName());
    }
}

void luaVm::CloseUpvalues(int a){
    vector<int> kv;
    for(auto p : state.stack->openuvs){
        kv.push_back(p.first);
    }
    for(auto key : kv){
        if(key >= a - 1){
            TValue * p = new TValue(*(state.stack->openuvs[key]->get()));
            state.stack->openuvs[key]->set(p);
            state.stack->openuvs.erase(key);
        }
    }
}