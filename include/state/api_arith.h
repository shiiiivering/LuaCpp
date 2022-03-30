#ifndef API_ARITH_H
#define API_ARITH_H

#include "../common/lua.h"
#include "../common/lua_object.h"
#include "../number/lua_math.h"
#include <cmath>

lua_Integer     iadd(lua_Integer a, lua_Integer b)  {return a + b;}
lua_Number      fadd(lua_Number a, lua_Number b)    {return a + b;}
lua_Integer     isub(lua_Integer a, lua_Integer b)  {return a - b;}
lua_Number      fsub(lua_Number a, lua_Number b)    {return a - b;}
lua_Integer     imul(lua_Integer a, lua_Integer b)  {return a * b;}
lua_Number      fmul(lua_Number a, lua_Number b)    {return a * b;}
lua_Integer     (*imod)(lua_Integer a, lua_Integer b) = IMod;
lua_Number      (*lua_fmod)(lua_Number a, lua_Number b)= FMod; // avoid conflict to fmod in cmath
lua_Number      lua_pow(lua_Number a, lua_Number b) {return pow(a, b);} // change name to make difference from pow from cmath
lua_Number      lua_div(lua_Number a, lua_Number b)     {return a / b;}
lua_Integer     (*iidiv)(lua_Integer a, lua_Integer b)= IFloorDiv;
lua_Number      (*fidiv)(lua_Number a, lua_Number b)  = FFloorDiv;
lua_Integer     band(lua_Integer a, lua_Integer b)  {return a & b;}
lua_Integer     bor(lua_Integer a, lua_Integer b)   {return a | b;}
lua_Integer     bxor(lua_Integer a, lua_Integer b)  {return a ^ b;}
lua_Integer     (*shl)(lua_Integer a, lua_Integer b)  = ShiftLeft;
lua_Integer     (*shr)(lua_Integer a, lua_Integer b)  = ShiftRight;
lua_Integer     iunm(lua_Integer a, lua_Integer _)  {return -a;}
lua_Number      funm(lua_Number a, lua_Number _)    {return -a;}
lua_Integer     bnot(lua_Integer a, lua_Integer _)    {return ~a;}

typedef struct luaOperator{
    string metamethod;
    lua_Integer (*integerFunc) (lua_Integer a, lua_Integer  b);
    lua_Number (*floatFunc) (lua_Number a, lua_Number b);
}luaOperator;

extern luaOperator luaOperators[];


#endif