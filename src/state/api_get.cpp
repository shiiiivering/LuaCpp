#include "lua_state.h"
#include "lua_table.h"
#include "vm/lua_vm.h"
#include <iostream>
using namespace std;

bool luaState::IsCppFunction(int idx){
    TValue val = stack->get(idx);
    if(val.type == LUA_TFUNCTION && val.value.c->cppFunc){
        return true;
    }
    else return false;
}

CppFunction luaState::ToCppFunction(int idx){
    TValue val = stack->get(idx);
    if(val.type == LUA_TFUNCTION){
        return val.value.c->cppFunc;
    }
    return nullptr;
}

void luaState::CreateTable(int nArr, int nRec){
    luaTable * t = new luaTable(nArr, nRec);
    stack->push(TValue(t));
}

void luaState::NewTable(){
    CreateTable(0, 0);
}

int luaState::getTable(const TValue & t, const TValue & k, bool raw){ 
    if(t.type == LUA_TTABLE){
        // if raw is true, we should ignore meta method __idx
        TValue v = t.value.t->get(k);
        if(raw || v.type != LUA_TNIL || !(hasMetafield(t, string("__index")))){
            stack->push(v);
            return v.type;
        }
    }
    if(!raw){
        TValue metafield = getMetafield(t, string("__index"));
        if(metafield.type != LUA_TNIL){
            switch(metafield.type){
                case LUA_TTABLE:
                    return getTable(metafield, k, false);
                case LUA_TFUNCTION:
                    Push(metafield);
                    Push(t);
                    Push(k);
                    vm->Call(2, 1);
                    TValue v = stack->get(-1);
                    return v.type;
            }
        }
    }
    panic("Index error!!! raw object must be a table");
}

int luaState::GetTable(int idx){
    // get value of a table
    // stack top stores the key. Table
    // stores in stack[idx];
    // returns result's value type
    TValue t = stack->get(idx);
    TValue k = stack->top();
    stack->pop();
    return getTable(t, k, false);
}

int luaState::RawGet(int idx){
    TValue t = stack->get(idx);
    TValue k = stack->top();
    stack->pop();
    return getTable(t, k, true);
}

int luaState::RawGetI (int idx, lua_Integer i){
    TValue t = stack->get(idx);
    return getTable(t, TValue(i), true);
}

int luaState::GetField(int idx, const string & k){
    // similar to GetTable. But key is string from parameter
    // a more standard but time consuming method is pushing 
    // k into stack and call GetTable(idx)
    TValue t = stack->get(idx);
    return getTable(t, TValue(k), false);
}

int luaState::GetI(int idx, lua_Integer i){
    TValue t = stack->get(idx);
    return getTable(t, TValue(i), false);
}

void luaState::setTable(const TValue & t, const TValue & k, const TValue & v, bool raw){
    if(t.type == LUA_TTABLE){
        if(raw || t.value.t->get(k).type != LUA_TNIL || !hasMetafield(t, "__newindex")){
            t.value.t->put(k, v);
            return;
        }
    }
    if(!raw){
        TValue metafield = getMetafield(t, "__newindex");
        if(metafield.type != LUA_TNIL){
            switch(metafield.type){
                case LUA_TTABLE:
                    setTable(metafield, k, v, false);
                    return;
                case LUA_TFUNCTION:
                    Push(metafield);
                    Push(t);
                    Push(k);
                    Push(v);
                    vm->Call(3, 0);
                    return;
            }
        }
    }
    panic("index error! only can set table of table or type with '__index' field in its metatable");
}

void luaState::SetTable(int idx){
    TValue t = stack->get(idx);
    TValue v = stack->top();
    stack->pop();
    TValue k = stack->top();
    stack->pop();
    setTable(t, k, v, false);
}

void luaState::SetField(int idx, const string & k){
    // use string instead of value from stack top
    TValue t = stack->get(idx);
    TValue v = stack->top();
    stack->pop();
    setTable(t, TValue(k), v, false);
}

void luaState::SetI(int idx, lua_Integer k){
    // modify table value int array indexing by k;
    TValue t = stack->get(idx);
    TValue v = stack->top();
    stack->pop();
    setTable(t, TValue(lua_Integer(k)), v, false);
}

void luaState::RawSet(int idx){
    TValue t = stack->get(idx);
    TValue v = stack->top();
    stack->pop();
    TValue k = stack->top();
    stack->pop();
    setTable(t, k, v, true);
}
void luaState::RawSetI(int idx, lua_Integer k){
    // modify table value int array indexing by k;
    TValue t = stack->get(idx);
    TValue v = stack->top();
    stack->pop();
    setTable(t, TValue(lua_Integer(k)), v, true);
}

void luaState::printTable(TValue v){
    auto table = v.value.t;
    if(table == nullptr){
        cout << "table is NULL" << endl;
        return;
    }
    cout << "arr: " << endl;
    for(auto i : table->arr){
        i.print();
    }
    cout << endl;
    cout << "map: " << endl;
    for(auto i : table->_map){
        cout << "(";
        i.first.print();
        cout << ", ";
        i.second.print();
        cout << ")";
    }
    cout << endl;
}


int luaState::GetGlobal(const TValue & name){
    TValue table = registry.value.t->get(TValue((lua_Integer)LUA_RIDX_GLOBALS));
    return getTable(table, name, true);
}

void luaState::SetGlobal(const TValue &  name){
    TValue table = registry.value.t->get(TValue((lua_Integer)LUA_RIDX_GLOBALS));
    TValue v = stack->top();
    stack->pop();
    setTable(table, name, v, true);
}

void luaState::Register(const TValue & name, CppFunction f){
    PushCppFunction(f);
    SetGlobal(name);
}

// metatable

bool luaState::hasMetafield(const TValue & a, const TValue & key){
    bool result = a.type == LUA_TTABLE && a.value.t->metatable.type != LUA_TNIL && a.value.t->metatable.value.t->get(key).type != LUA_TNIL;
    return result;
}