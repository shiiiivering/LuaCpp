#include "lua_state.h"
#include "lua_table.h"
#include <string>
using namespace std;

void luaState::Len(int idx){
    TValue v = stack->get(idx);
    if(v.type == LUA_LNGSTR || v.type == LUA_SHRSTR){
        PushInteger((lua_Integer)(v.value.str->size()));
        return;
    }
    int status = LUA_OK;
    TValue result = callMetamethod(v, v, string("__len"), & status);
    if(status == LUA_OK){
        Push(result);
    }
    else if (v.type == LUA_TTABLE){
        PushInteger((lua_Integer)(v.value.t->len()));
    }
    else{
        // now we consider string's length only
        panic("length error!!! Only String and Table objects have length");
    }
}

int luaState::RawLen(int idx){
    TValue val = stack->get(idx);
    switch(val.type){
        case LUA_LNGSTR:
        case LUA_SHRSTR:
            return (*(val.value.str)).size();
        case LUA_TTABLE:
            return val.value.t->len();
        default:
            return 0;
    }
}

void luaState::Concat(int n){
    if(n == 0){
        Push(TValue(string("")));
    }
    else if(n >= 2){
        for(int i = 1; i < n; i++){
            if(IsString(-1) && IsString(-2)){
                string s = ToString(-2) + ToString(-1);
                Pop(2);
                PushString(s);
                continue;
            }
            // call metamethod
            TValue b = stack->top();
            stack->pop();
            TValue a = stack->top();
            stack->pop();
            int status = LUA_OK;
            TValue result = callMetamethod(a, b, "__concat", &status);
            if(status == LUA_OK){
                Push(result);
                continue;
            }
            
            panic("concat error!!! Can only concat string value");
            
        }
    }
    else{
        // does nothing when n == 1
        // stack top is the already the result
    }
}

bool luaState::Next(int idx){
    // get key on top of stack and table at idx.
    // pop old key and push next key and value to
    // stack top;
    TValue val = stack->get(idx);
    if(val.type == LUA_TTABLE){
        TValue key = stack->top();
        stack->pop();
        TValue nextKey = val.value.t->nextKey(key);
        if(nextKey.type != LUA_TNIL){
            Push(nextKey);
            Push(val.value.t->get(nextKey));
            return true;
        }
        return false;
    }
    panic("can only iterate on table object");
}