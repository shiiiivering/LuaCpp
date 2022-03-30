#include "lua_math.h"
#include "math.h"
#include "common/lua.h"

lua_Integer IFloorDiv(lua_Integer a, lua_Integer b){
    if(a > 0 && b > 0 || a < 0 && b < 0 || a % b == 0){
        return a / b;
    }
    else{
        return a / b - 1;
    }
}

lua_Number FFloorDiv(lua_Number a, lua_Number b){
    return floor(a / b);
}

lua_Integer IMod(lua_Integer a, lua_Integer b){
    return a - IFloorDiv(a, b) * b;
}
lua_Number FMod(lua_Number a, lua_Number b){
    return a - floor(a / b) * b;
}

lua_Integer ShiftLeft(lua_Integer a, lua_Integer n){
    return a << n;
}
lua_Integer ShiftRight(lua_Integer a, lua_Integer n){
    return a >> n;
}

lua_Integer FloatToInteger(lua_Number f, int * status){
    lua_Integer i = (lua_Integer)(round(f));
    if(FLOAT_INTEGER_EQUAL(f, i)){
        if(status) *status = LUA_OK;
        return i;
    }
    else{
        if(status) *status = LUA_ERROR;
        return i;
    }
}

lua_Integer ParseInteger(string & str, int * status){
    int x;
    try{
        x = stoll(str);
    }
    catch(...){
        if(status)*status = LUA_ERROR;
        return 0;
    }
    if(status)*status = LUA_OK;
    return x;
}
lua_Number ParseFloat(string & str, int * status){
    lua_Number x;
    try{
        x = stod(str);
    }
    catch(...){
        if(status)*status = LUA_ERROR;
        return 0.0;
    }
    if(status)*status = LUA_OK;
    return x;
}