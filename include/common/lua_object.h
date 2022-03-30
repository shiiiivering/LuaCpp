#ifndef LUA_OBJECT_H
#define LUA_OBJECT_H
// lua type system
#include "lua.h"
#include <string>
using namespace std;

// TODO:
// garbage collection
// for string: just copy with TValue
// garbage collection of table and function(closure) is important

class luaTable;
class luaClosure;
typedef union lua_Value{
    std::string * str;
    luaTable * t;
    luaClosure * c;
    void * p;
    int b;
    lua_Integer i;
    lua_Number n;
    // lua_CFunction f;
}Value;

class TValue{
    public:
    Value value;
    int type;
    friend class binaryChunk;
    public:
    ~TValue();
    TValue():type(LUA_TNIL){}
    TValue(bool b):type(LUA_TBOOLEAN){value.b = b;}
    TValue(lua_Integer i):type(LUA_NUMINT){value.i = i;}
    TValue(lua_Number n):type(LUA_NUMFLT){value.n = n;}
    TValue(const string & str):type(LUA_TSTRING){
        value.str = new string(str);
    }
    TValue (const char * chs):type(LUA_TSTRING){
        value.str = new string(chs);
    }
    TValue(luaTable * t);
    TValue(luaClosure * c);

    TValue(const TValue & old);
    TValue(TValue && old);

    TValue & operator = (TValue && old);
    TValue & operator = (const TValue & old);

    bool operator == (const TValue & p) const;

    lua_Number convertToFloat(int * status=nullptr) const;
    lua_Integer convertToInteger(int * status=nullptr) const;

    void print() const ;
    void printTable() const;
    private:
    lua_Integer _stringToInteger(int * status=nullptr) const;
};

class TValueHash{
    public:
    size_t operator()(const TValue & a) const noexcept;
};

#endif