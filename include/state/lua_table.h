#ifndef LUA_TABLE_H
#define LUA_TABLE_H

#include "../common/lua.h"
#include "../common/lua_object.h"
#include "number/lua_math.h"
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

/*************************************
 * lua array index begins with 1. So
 * if you want to visit table[index] 
 * in lua, use arr[index - 1] in array.
 * if element not in array, please use 
 * map[index].
 * ***********************************/

/*****************************
 * TODO: Garbage Collection
 * *************************/

class luaTable{
    public:
    typedef unordered_map<TValue, TValue, TValueHash> tvalue_Map;

    vector<TValue> arr; // table used as array
    tvalue_Map _map;    // table used as map
    tvalue_Map keys;    // for_loop keys[key] = next(key);
    bool changed;
    TValue metatable; // metatable
    int ref;
    private: 
    void shrinkArray();
    void expandArray();
    public:
    luaTable(int nArr, int nRec);
    luaTable():ref(0), changed(true){};

    int len(){return arr.size();}
    TValue get(const TValue & key);
    void put(const TValue & key, const TValue & val); 

    // for loop
    TValue nextKey(TValue key) ;
    void initKeys();
};

#endif