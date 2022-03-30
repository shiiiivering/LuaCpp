#include "lua_stack.h"
#include "lua_state.h"
#include "common/lua.h"

luaStack::luaStack():pc(0), closure(nullptr), prev(nullptr), state(nullptr){
    slots = vector<TValue>();
    slots.reserve(LUA_STACKSIZE);
}

luaStack::luaStack(int stackSize):pc(0), closure(nullptr), prev(nullptr), state(nullptr){
    slots = vector<TValue>();
    slots.reserve(stackSize);
}
luaStack::luaStack(int stackSize, luaState * state):pc(0), closure(nullptr), prev(nullptr), state(state){
    slots = vector<TValue>();
    slots.reserve(stackSize);
}

void luaStack::push(const TValue & v){
    slots.emplace_back(v);
}
void luaStack::push(){
    slots.emplace_back(TValue());
}

void luaStack::pop(){
    slots.pop_back();
}

vector<TValue> luaStack::popN(int n){
    vector<TValue> result(n, TValue());
    for(int i = n - 1; i >= 0; i--){
        result[i] = top();
        pop();
    }
    return result;
}

void luaStack::pushN(vector<TValue> v, int n){
    int nVals = v.size();
    if(n < 0){
        n = nVals;
    }
    for(int i = 0; i < n; i ++){
        if(i < nVals){
            push(v[i]);
        }
        else{
            push();
        }
    }
}

TValue luaStack::top(){
    return slots[slots.size() - 1];
}

int luaStack::topIndex(){
    return slots.size();
}

int luaStack::absIndex(int idx){
    if(idx >= 0){
        return idx;
    }
    else if(idx <= LUA_REGISTRYINDEX){ // register table index
        return idx;
    }
    else{
        return idx + slots.size() + 1;
    }
}

bool luaStack::isValid(int idx){
    int abs_idx = absIndex(idx);
    if(idx < LUA_REGISTRYINDEX){ // upvalues
        int uvIdx = LUA_REGISTRYINDEX - idx - 1;
        return closure != nullptr && uvIdx < closure->upvals.size();
    }
    if(abs_idx == LUA_REGISTRYINDEX){
        return true;
    }
    return abs_idx > 0 && abs_idx <= slots.size();
}

void luaStack::set(int idx, TValue v){
    int abs_idx = absIndex(idx);
    if(abs_idx < LUA_REGISTRYINDEX){
        int uvIdx = LUA_REGISTRYINDEX - idx - 1;
        if(isValid(abs_idx)){
            *(closure->upvals[uvIdx]->get()) = v;
        }
        return;
    }
    if(abs_idx == LUA_REGISTRYINDEX){
        state->registry = v;
        return;
    }
    if(abs_idx > 0 && abs_idx <= slots.size()){
        slots[abs_idx - 1] = std::move(v);
        return;
    }
    else{
        panic("invalid index !!!");
    }
}
TValue luaStack::get(int idx){
    int abs_idx = absIndex(idx);
    if(abs_idx < LUA_REGISTRYINDEX){ // upvalues
        int uvIdx = LUA_REGISTRYINDEX - abs_idx - 1;
        if(!isValid(abs_idx)){
            return TValue();
        }
        return *(closure->upvals[uvIdx]->get());
    }
    if(abs_idx == LUA_REGISTRYINDEX){
        return state->registry;
    }
    if(abs_idx > 0 && abs_idx <= slots.size()){
        return slots[abs_idx - 1];
    }
    return TValue();
}

int luaStack::getType(int idx){
    int abs_idx = absIndex(idx);
    if(abs_idx > 0 && abs_idx <= slots.size()){
        return slots[abs_idx - 1].type;
    }
    return LUA_TNONE;
}

void luaStack::reverse(int from, int to){
    while(from < to){
        swap(slots[from], slots[to]);
        from++;
        to--;
    }
}