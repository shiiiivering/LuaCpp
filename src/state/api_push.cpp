#include "lua_state.h"
#include <memory>
using namespace std;
// push operations
void luaState::PushNil(){stack->push(TValue());}
void luaState::PushBoolean(bool b){stack->push(TValue(b));}
void luaState::PushInteger(lua_Integer i){stack->push(TValue(i));}
void luaState::PushNumber(lua_Number n){stack->push(TValue(n));}
void luaState::PushString(const string & s){stack->push(TValue(s));}
void luaState::Push(const TValue & v){stack->push(v);}

void luaState::PushGlobalTable(){
    TValue global = registry.value.t->get(TValue((lua_Integer)LUA_RIDX_GLOBALS));
    stack->push(global);
}

void luaState::PushCppFunction(CppFunction f){
    stack->push(TValue(new luaClosure(f)));
}

void luaState::PushCppClosure(CppFunction f, int n ){
    luaClosure * closure = new luaClosure(f, n);
    for(int i = n; i > 0; i--){
        TValue val = stack->top();
        stack->pop();
        closure->cppFuncUpvalCache.push_back(val);
        closure->upvals[i - 1] = shared_ptr<upvalue>(new upvalue(&(closure->cppFuncUpvalCache[i - 1])));
    }
    stack->push(closure);
}