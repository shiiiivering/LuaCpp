#ifndef LUA_STACK_H
#define LUA_STACK_H

#include "../common/lua_object.h"
#include "../common/lua.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include "lua_closure.h"
using namespace std;

class luaState;
class luaStack{
    // **index of luaStack begins with 1**
    public: 
    vector<TValue> slots;
    luaClosure * closure;
    vector<TValue> varargs;
    int pc;
    luaState * state; // to visit register table
    unordered_map<int, shared_ptr<upvalue>> openuvs;
    public:
    luaStack * prev;


    luaStack();
    luaStack(int stackSize);
    luaStack(int stackSize, luaState * state);
    // stack operations
    void check(int n){};      // without extra stack now
                            // not inplement now. memory management hands on to
                            // STL vector now. top is slots.size();
    void push(const TValue & v);
    void push();
    void pop();             // personally, I'd like the pop function returns nothing,
                            // and add a new top() function to return the top value;
    vector<TValue> popN(int n);
    void pushN(vector<TValue> v, int n);
    TValue top();           // this method will make a deep copy for the TValue ibject
                            // which will be expensive for long string type.
                            // so try to only use it when need to obtain value when
                            // doing stack pop.
    int topIndex();
    int absIndex(int idx);  // with out considering validation of index
    bool isValid(int idx);  // index of lua stack begins with 1 instead of 0
    void set(int idx, TValue v);
    TValue get(int idx);    // return a copy of value
                            // simply deep copy now
                            // return nil if index is invalid
    int getType(int idx);
    void reverse(int from, int to); // reverse values in stack between from and to

};

#endif