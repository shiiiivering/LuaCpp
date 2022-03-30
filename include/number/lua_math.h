#ifndef LUA_MATH_H
#define LUA_MATH_H

#include "../common/lua.h"
// #include "../common/lua_object.h"
#include <string>
using namespace std;

// integer division
lua_Integer IFloorDiv(lua_Integer a, lua_Integer b);
lua_Number FFloorDiv(lua_Number a, lua_Number b); // float division. Is math.floor really needed? Return float here.

// mod
lua_Integer IMod(lua_Integer a, lua_Integer b);
lua_Number FMod(lua_Number a, lua_Number b);

// bit operation
lua_Integer ShiftLeft(lua_Integer a, lua_Integer n);
lua_Integer ShiftRight(lua_Integer a, lua_Integer n);

// type cast
lua_Integer FloatToInteger(lua_Number f, int * status=nullptr);

// parse string
// use stoll and stod from <string> here
lua_Integer ParseInteger(string & str, int * status=nullptr);
lua_Number ParseFloat(string & str, int * status=nullptr);
#endif