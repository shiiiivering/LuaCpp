#include "lua_vm.h"
#include <unordered_map>

void luaVm::Call(int nArgs, int nResults){
    TValue val = state.stack->get(-(nArgs + 1)); // get function place
    if(val.type != LUA_TFUNCTION){
        TValue metafield = state.getMetafield(val, "__call");
        if(metafield.type == LUA_TFUNCTION){
            state.Push(val);
            state.Insert(-(nArgs + 2));
            nArgs += 1;
            val = metafield;
        }
    }
    if(val.type == LUA_TFUNCTION){
        if(val.value.c->proto != nullptr){
            callLuaClosure(nArgs, nResults, val);
        }else{
            callCppClosure(nArgs, nResults, val);
        }
    }
    else{
        panic("trying to call a non-function value!");
    }
}

void luaVm::callLuaClosure(int nArgs, int nResults, TValue & val){
    state.pushLuaClosure(nArgs, val);
    runLuaClosure();
    state.popLuaClosure(nResults, val);
}

void luaVm::callCppClosure(int nArgs, int nResults, TValue & val){
    state.callCppClosure(nArgs, nResults, val);
}

void luaVm::runLuaClosure(){
    // main loop of executing a function
    while(true){
        Instruction inst(Fetch());
        // cout << endl;
        // cout << inst.GetOpName() << " ";
        Execute(inst);

        // state.printStack();
        // cout << endl;
        // for(int i = 0; i < state.stack->slots.size(); i++){
        //     if(state.stack->slots[i].type == LUA_TTABLE){
        //         cout << "print table at slots ----------------------------" << i << endl;
        //         state.stack->slots[i].printTable();
        //     }
        // }

        if(inst.Opcode() == OP_RETURN){
            break;
        }
    }
}

void luaVm::closure(Instruction inst){
    vector<int> abx = inst.ABx();
    abx[0] ++;
    LoadProto(abx[1]);
    state.Replace(abx[0]);
}

void luaVm::call(Instruction inst){
    // R(A), ..., R(A + C - 2) = R(A)(R(A + 1) + ... + R(A + B - 1))
    vector<int> abc = inst.ABC();
    abc[0]++;
    int nArgs = PushFuncAndArgs(abc[0], abc[1]);
    Call(nArgs, abc[2] - 1);
    PopResults(abc[0], abc[2]);
}

int luaVm::PushFuncAndArgs(int a, int b){
    if(b >= 1){
        for(int i = a; i < a + b; i++){
            state.PushValue(i);
        }
        return b - 1;
    }
    else{
        // b == 0
        // accept all the return value from function call parameters.
        FixStack(a);
        return state.GetTop() - RegisterCount() - 1;
    }
}

void luaVm::FixStack(int a){
    int x = state.ToInteger(-1);
    state.Pop(1);
    for(int i = a; i < x; i++){
        state.PushValue(i);
    }
    state.Rotate(RegisterCount() + 1, x - a);
}

void luaVm::PopResults(int a, int c){
    if (c == 1){
        // no results, does nothing
    }
    else if(c > 1){ // c - 1 results
        for(int i = a + c - 2; i >= a; i--){
            state.Replace(i);
        }
    }else{
        state.PushInteger(lua_Integer(a));
    }
}

void luaVm::vm_return(Instruction inst){
    vector<int> abc = inst.ABC();
    abc[0] ++;

    if(abc[1] == 1){ // no return values

    }
    else if(abc[1] > 1){
        // b - 1 results
        for(int i = abc[0]; i <= abc[0] + abc[1] - 2; i++){
            state.PushValue(i);
        }
    }else{
        FixStack(abc[0]);
    }
}

void luaVm::vararg(Instruction inst){
    vector<int> abc = inst.ABC();
    abc[0] ++;

    if(abc[1] != 1){
        LoadVararg(abc[1] - 1);
        PopResults(abc[0], abc[1]);
    }
}

void luaVm::tailcall(Instruction inst){
    // just call as normal function
    // TODO: tailcall advanced implementation
    vector<int> abc = inst.ABC();
    abc[0] ++;
    abc[2] = 0;
    int nArgs = PushFuncAndArgs(abc[0], abc[1]);
    Call(nArgs, abc[2] - 1);
    PopResults(abc[0], abc[2]);
}

void luaVm::self(Instruction inst){
    // R(a + 1) = R(b); R(a) = R(b) [RK(c)]
    vector<int> abc = inst.ABC();
    abc[0]++;
    abc[1]++;
    state.Copy(abc[1], abc[0] + 1);
    GetRK(abc[2]);
    state.GetTable(abc[1]);
    state.Replace(abc[0]);
}

int luaVm::RegisterCount(){
    return state.stack->closure->proto->MaxStackSize;
}

void luaVm::LoadVararg(int n){
    if(n < 0){
        n = state.stack->varargs.size();
    }
    state.stack->pushN(state.stack->varargs, n);
}

void luaVm::LoadProto(int idx){
    Prototype * proto = state.stack->closure->proto->Protos[idx];
    luaClosure * closure = new luaClosure(proto);
    state.Push(TValue(closure));
    for(int i = 0; i < proto->Upvalues.size(); i++){
        int uvIdx = proto->Upvalues[i].Idx;
        if(proto->Upvalues[i].Instack == 1){
            if(state.stack->openuvs.count(uvIdx)){
                closure->upvals[i] = state.stack->openuvs[uvIdx];
            }
            else{
                closure->upvals[i] = shared_ptr<upvalue>(new upvalue(&(state.stack->slots[uvIdx])));
                state.stack->openuvs[uvIdx] = closure->upvals[i];
            }
        } else { // capture outer local values, which has been captured by this closure.
            closure->upvals[i] = state.stack->closure->upvals[uvIdx];
        }
    }
}

void luaVm::tForCall(Instruction inst){
    vector<int> abc = inst.ABC();
    abc[0]++;
    PushFuncAndArgs(abc[0], 3);
    Call(2, abc[2]);
    PopResults(abc[0] + 3, abc[2] + 1);
}