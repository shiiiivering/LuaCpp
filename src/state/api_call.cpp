#include "lua_state.h"
#include "lua_closure.h"
#include "binchunk/binary_chunk.h"
#include "common/lua_object.h"
int luaState::Load(string path, string name, string mode){
    cout << path << endl;
    binaryChunk bc(path);
    Prototype * proto = bc.Undump();
    bc.printList();
    luaClosure * c = new luaClosure(proto);
    stack->push(TValue(c));

    if(proto->Upvalues.size() > 0){ // set _ENV
        TValue * env =new TValue(registry.value.t->get(TValue(lua_Integer(LUA_RIDX_GLOBALS))));
        c->upvals[0] = shared_ptr<upvalue>(new upvalue(env));
        c->upvals[0]->set(env); // close upval, to make sure 
                                // the new env copy will be 
                                // released after execution
    }

    return 0;
}

// void luaState::Call(int nArgs, int nResults){
//     // TValue val = stack->get(-(nArgs + 1)); // get function place
//     // if(val.type == LUA_TFUNCTION){
//     //     printf("call %s<%d, %d>\n", val.value.c->proto->Source, val.value.c->proto->LineDefined, val.value.c->proto->LastLineDefined);
//     //     callLuaClosure(nArgs, nResults, val);
//     // }
//     // else{
//     //     panic("trying to call a non-function value!");
//     // }
// }

void luaState::pushLuaClosure(int nArgs, TValue & val){
    luaClosure * c = val.value.c;
    int nRegs = c->proto->MaxStackSize;
    int nParams = c->proto->NumParams;
    bool isVararg = c->proto->IsVararg == 1;

    luaStack * newStack = new luaStack(nRegs + LUA_MINSTACK);
    newStack->closure = c;

    vector<TValue> funcAndArgs = stack->popN(nArgs + 1);
    newStack->pushN(vector<TValue>(funcAndArgs.begin() + 1, funcAndArgs.end()), nArgs);
    newStack->pushN(vector<TValue>(0), nRegs - newStack->topIndex());
    if((nArgs > nParams) && isVararg){
        newStack->varargs.assign(funcAndArgs.begin() + nParams + 1, funcAndArgs.end());
    }
    pushLuaStack(newStack);
}

void luaState::popLuaClosure(int nResults, TValue & val){
    luaStack * newStack = stack;
    int nRegs = val.value.c->proto->MaxStackSize;
    popLuaStack();
    if(nResults != 0){
        vector<TValue> results = newStack->popN(newStack->topIndex() - nRegs);
        stack->pushN(results, nResults);
    }
    delete newStack;
}

void luaState::callCppClosure(int nArgs, int nResults, TValue & val){
    luaStack * newStack = new luaStack(nArgs + LUA_MINSTACK);
    newStack->closure = val.value.c;

    vector<TValue> args = stack->popN(nArgs);
    newStack->pushN(args, nArgs);
    stack->pop(); // throw the cpp function closure from stack directly
    pushLuaStack(newStack);
    int ret = val.value.c->cppFunc(this);
    popLuaStack();

    if(nResults != 0){
        vector<TValue> results = newStack->popN(ret);
        stack->pushN(results, nResults);
    }
}