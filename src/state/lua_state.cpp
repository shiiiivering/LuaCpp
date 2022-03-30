#include "lua_state.h"
#include "vm/lua_vm.h"
#include <iostream>
#include <memory>
#include <string>
#include "common/lua.h"
using namespace std;

luaState::luaState(luaVm * vm): vm(vm){
    stack = new luaStack(LUA_MINSTACK);
    registry = TValue(new luaTable());
    registry.value.t->ref++;
    registry.value.t->put(TValue((lua_Integer)LUA_RIDX_GLOBALS), TValue(new luaTable(0, 0)));
}

void luaState::printStack(){
    int top = GetTop();

    for(int i = 1; i <= top; i++){
        int t = Type(i);
        switch (t){
            case LUA_TBOOLEAN: printf("[%s]", ToBoolean(i) ? "true" : "false");
            break;
            case LUA_NUMFLT: printf("[%f]", ToNumber(i));
            break;
            case LUA_NUMINT: printf("[%d]", ToInteger(i));
            break;
            case LUA_TNIL: printf("[%s]", "nil");
            break;
            case LUA_SHRSTR: 
            case LUA_LNGSTR:
                printf("[\"%s\"]", ToString(i).c_str());
                break;
            default: {
                stack->slots[i - 1].print();
            }
        }
    }
    cout << endl;
}

// stack operations
int luaState::GetTop(){
    return stack->topIndex();
}

int luaState::AbsIndex(int idx){
    return stack->absIndex(idx);
}

void luaState::Pop(int n){
    for(int i = 0; i < n; i++){
        stack->pop();
    }
}
void luaState::Copy(int src, int tar){
    TValue v(stack->get(src));
    stack->set(tar, v);
}
void luaState::PushValue(int idx){
    TValue v(stack->get(idx));
    stack->push(v);
}
void luaState::Replace(int idx){
    TValue temp = stack->top();
    stack->pop();
    stack->set(idx, std::move(temp));
}
void luaState::Insert(int idx){
    Rotate(idx, 1);
}
void luaState::Remove(int idx){
    Rotate(idx, -1);
    Pop(1);
}
void luaState::Rotate(int idx, int n){
    int t = stack->topIndex() - 1;
    int p = AbsIndex(idx) - 1;
    int m = (n >= 0 ? t - n : p - n - 1);
    stack->reverse(p, m);
    stack->reverse(m+1, t);
    stack->reverse(p, t);
}
void luaState::SetTop(int idx){
    int new_top = AbsIndex(idx);
    if(new_top < 0){
        panic("stack underflow");
    }
    int n = stack->topIndex() - new_top;
    if(n > 0){
        for(int i = 0; i < n; i++){
            stack->pop();
        }
    }
    else if(n < 0){
        for(int i = 0; i > n; i--){
            stack->push();
        }
    }
}

// push operations

// access operations
string luaState::TypeName(int type){
    switch(type){
        case LUA_TNONE: return "no value";
        case LUA_TNIL: return "nil";
        case LUA_TBOOLEAN: return "boolean";
        case LUA_TNUMBER: return "number";
        case LUA_NUMINT: return "integer";
        case LUA_TSTRING: return "string";
        case LUA_TTABLE: return "table";
        case LUA_TFUNCTION: return "function";
        case LUA_TTHREAD: return "thread";
        default: return "userdat";
    };
}
int luaState::Type(int idx){
    return stack->getType(idx);
}

bool luaState::IsNumber(int idx){
    int status;
    ToNumberX(idx, &status);
    return status == LUA_OK;
}

bool luaState::IsInteger(int idx){
    return Type(idx) == LUA_NUMINT;
}

bool luaState::ToBoolean(int idx){
    TValue val = stack->get(idx);
    return convertToBoolean(val);
}

bool luaState::convertToBoolean(const TValue & v){
    switch (v.type){
        case LUA_TNIL: return false;
        case LUA_TBOOLEAN: return v.value.b;
        default: return true;
    }
}

lua_Number luaState::ToNumber(int idx){
    return ToNumberX(idx);
}
lua_Number luaState::ToNumberX(int idx, int * status){
    // simple implementation
    // TODO: Will Complete it Later 
    TValue x = stack->get(idx);
    return x.convertToFloat(status);
}

lua_Integer luaState::ToInteger(int idx){
    return ToIntegerX(idx);
}
lua_Integer luaState::ToIntegerX(int idx, int * status){
    TValue v = stack->get(idx);
    return v.convertToInteger(status);
} 

string luaState::ToString(int idx){
    return ToStringX(idx);
}

string luaState::ToStringX(int idx, int * status){
    TValue v = stack->get(idx);
    string s;
    switch (v.type){
        case LUA_SHRSTR:
        case LUA_LNGSTR:
            if(status) *status = LUA_OK; return *(v.value.str);
        case LUA_NUMINT:
            s = to_string(v.value.i);
            stack->set(idx, TValue(s));
            if(status) *status = LUA_OK; return s;
        case LUA_NUMFLT:
            s = to_string(v.value.n);
            stack->set(idx, TValue(s));
            if(status) *status = LUA_OK; return s;
        default:
            if(status) *status = LUA_ERROR; return "";
    }
}

void luaState::pushLuaStack(luaStack * new_stack){
    new_stack->prev = stack;
    stack = new_stack;
}
void luaState::popLuaStack(){
    luaStack * old_stack = stack;
    for(pair<const int, shared_ptr<upvalue>> & p : stack->openuvs){
        // openuvs means value in stack and avaliable now
        // so just reallocate them when destroying a stack
        // and close the upvalue(automatically done when set.),
        // to destory the TValue when upvalue released
        p.second->set(new TValue(*(p.second->get())));
        // for(int i = 0; i < stack->slots.size(); i++){
        //     if(&(stack->slots[i]) == p.second->get()){
        //         p.second->set(new TValue(*(p.second->get())));
        //     }
        // }
    }
    stack = stack->prev;
    old_stack->prev = nullptr;
}

// metatable
void luaState::setMetatable(TValue val, TValue mt){
    if(mt.type != LUA_TTABLE){
        panic("Metatable must be a TABLE VALUE");
    }
    if(val.type == LUA_TTABLE){
        val.value.t->metatable = mt;
        return;
    }
    string key = TYPE_METATABLE_NAME(val);
    registry.value.t->put(TValue(key), mt); // set shared metatable for a certain builtin type
}

TValue luaState::getMetatable(const TValue & val){
    if(val.type == LUA_TTABLE){
        return val.value.t->metatable;
    }
    TValue key(TYPE_METATABLE_NAME(val));
    TValue mt = registry.value.t->get(key);
    if(mt.type != LUA_TNIL){
        return mt;
    }
    return TValue();
}


TValue luaState::callMetamethod(TValue & a, TValue & b, const string & metamethodName, int * status){
    TValue metaMethod;
    metaMethod = getMetafield(a, metamethodName);
    if(metaMethod.type == LUA_TNIL){
        metaMethod = getMetafield(b, metamethodName);
        if(metaMethod.type == LUA_TNIL){
            if(status)*status = LUA_ERROR;
            return TValue();
        }
    }

    stack->push(metaMethod);
    stack->push(a);
    stack->push(b);
    vm->Call(2, 1);
    TValue result = stack->top();
    stack->pop();

    if(status)*status = LUA_OK;

    return result;
}

TValue luaState::getMetafield(const TValue & a, const TValue & fieldName){
    TValue metatable = getMetatable(a);
    if(metatable.type != LUA_TNIL){
        return metatable.value.t->get(fieldName);
    }
    return TValue();
}

bool luaState::GetMetatable(int idx){
    TValue val = stack->get(idx);
    TValue metatable = getMetatable(val);
    if(metatable.type != LUA_TNIL){
        stack->push(metatable);
        return true;
    }
    else{
        return false;
    }
}

void luaState::SetMetatable(int idx){
    TValue val = stack->get(idx);
    TValue mtVal = stack->top();
    stack->pop();
    if(mtVal.type == LUA_TNIL){
        setMetatable(val, TValue());
    }
    else if(mtVal.type == LUA_TTABLE){
        setMetatable(val, mtVal);
    }
    else{
        panic("metatable can only be table!!");
    }
}