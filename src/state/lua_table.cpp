#include "lua_table.h"
#include "common/lua_object.h"
using namespace std;

luaTable::luaTable(int nArr, int nRec):ref(0), changed(true){
    // to reserve space to enhance performance
    if(nArr > 0){
        arr = vector<TValue>(nArr);
    }
    if(nRec > 0){
        _map.reserve(nRec);
    }
}

TValue luaTable::get(const TValue & key){
    int status = LUA_ERROR;
    lua_Integer int_key = 0; 
    if(key.type == LUA_NUMFLT || key.type == LUA_NUMINT)
        int_key = key.convertToInteger(&status);
    // index of array begins with 1 in lua
    if(status == LUA_OK && int_key >= 1 && int_key <= arr.size()){
        return arr[int_key - 1];
    }
    return _map[key];
}

void luaTable::shrinkArray(){
    // deal with hole at the end of array
    while(arr[arr.size() - 1].type == LUA_TNIL){
        arr.pop_back();
    }
}

void luaTable::expandArray(){
    // deal with array expand
    // after dynamic expand of array, check map if there is any
    // elements can be push back to the tail of array
    lua_Integer idx = arr.size() + 1;
    while(_map.count(TValue(idx))){
        arr.push_back(_map[TValue(idx)]);
        _map.erase(TValue(idx));
        idx++;
    }
}

void luaTable::put(const TValue & key, const TValue & val){

    if(key.type == LUA_TNIL){
        panic("table index can not be NIL");
    }
    if(key.type == LUA_NUMFLT && key.value.n != key.value.n){
        panic("table index is NaN!");
    }
    int status;
    lua_Integer int_key = key.convertToInteger(&status);
    if(status == LUA_OK && int_key >= 1){
        int arr_len = arr.size();
        if(int_key <= arr_len){
            arr[int_key - 1] = val;
            if(int_key == arr_len && val.type == LUA_TNIL){
                shrinkArray();
            }
            return;
        }
        if(int_key == arr_len + 1){
            _map.erase(TValue(int_key));
            if(val.type != LUA_TNIL){
                arr.push_back(val);
                expandArray();
            }
            return;
        }
    }
    if(val.type != LUA_TNIL){
        _map[status == LUA_OK ? TValue(int_key) : key] = val;
    }
    else{
        _map.erase(status == LUA_OK ? TValue(int_key) : key);
    }
}

TValue luaTable::nextKey(TValue key) {
    if(key.type == LUA_TNIL){
        initKeys();
        changed = false;
    }
    return keys[key];
}

void luaTable::initKeys(){
    keys.clear();
    TValue key;
    for(int i = 0; i < arr.size(); i++){
        TValue p = arr[i];
        if(p.type != LUA_TNIL){
            keys[key] = TValue((lua_Integer)(i + 1));
            key = TValue((lua_Integer)(i + 1));
        }
    }
    for(auto p : _map){
        if(p.second.type != LUA_TNIL){
            keys[key] = p.first;
            key = p.first;
        }
    }
}