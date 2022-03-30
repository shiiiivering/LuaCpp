#ifndef LUA_H
#define LUA_H

#include <iostream>
#include <string>

#define LUA_INTEGER long long
#define LUA_NUMBER double 

// types
typedef LUA_INTEGER lua_Integer;
typedef LUA_NUMBER lua_Number;
typedef unsigned char lu_byte;
typedef int (*lua_CFunction)(); // TODO: add parameters here

#define LUA_MIN_FLOAT (1e-10)
#define FLOAT_INTEGER_EQUAL(f, i) (abs((f) - (i)) < LUA_MIN_FLOAT)


// lua status code
#define LUA_OK 0
#define LUA_ERROR 1

// LUA SUBTYPE BIT
#define LUA_SUBTYPE_BIT 4
#define LUA_MAX_TYPE_INDEX 70

// LUA BASIC VALUE TYPE
#define LUA_TNONE          -1
#define LUA_TNIL            0
#define LUA_TBOOLEAN        1
#define LUA_TLIGHTUSERDATA  2
#define LUA_TNUMBER         3
#define LUA_TSTRING         4
#define LUA_TTABLE          5
#define LUA_TFUNCTION       6
#define LUA_TUSERDATA       7
#define LUA_TTHREAD         8

// lua number type
#define SUBTYPE(o) (o << LUA_SUBTYPE_BIT)
#define LUA_NUMINT (LUA_TNUMBER | SUBTYPE(1))
#define LUA_NUMFLT (LUA_TNUMBER | SUBTYPE(0))

// lua function type
#define LUA_TLCL (LUA_TFUNCTION | SUBTYPE(0))
#define LUA_TLCF (LUA_TFUNCTION | SUBTYPE(1))
#define LUA_TCCL (LUA_TFUNCTION | SUBTYPE(2))

// string type
#define LUA_LNGSTR (LUA_TSTRING | SUBTYPE(1))
#define LUA_SHRSTR (LUA_TSTRING | SUBTYPE(0))




// LUA stack define
#define LUA_MINSTACK 20
#define LUA_STACKSIZE (2 * LUA_MINSTACK)
// #define LUA_EXTRASTACK 5
#define LUA_MAXSTACK 15000
#define LUA_ERRORSTACK 200
#define LUA_MULRET (-1)
#define LUA_MAXCALLS 200
#define LUA_REGISTRYINDEX (-LUA_MAXSTACK - 1000)
#define LUA_RIDX_GLOBALS 2


inline void panic(const std::string & s){
    std::cout << "ERROR: " << s << std::endl;
    exit(1);
}

// base operator
enum LuaType{

};

enum ArithOp{
    LUA_OPADD,      // +
    LUA_OPSUB,      // -
    LUA_OPMUL,      // *
    LUA_OPMOD,      // %
    LUA_OPPOW,      // ^
    LUA_OPDIV,      // /
    LUA_OPIDIV,     // //
    LUA_OPBAND,     // &
    LUA_OPBOR,      // |
    LUA_OPBXOR,     // ~
    LUA_OPSHL,      // <<
    LUA_OPSHR,      // >>
    LUA_OPUNM,      // - (unary)
    LUA_OPBNOT,     // ~
};

enum CompareOp{
    LUA_OPEQ,       // ==
    LUA_OPLT,       // <
    LUA_OPLE,       // <=
};

// For loop batch size
#define LFIELD_PER_FLUSH 50

// Cpp Function
class luaState;
typedef int(* CppFunction)(luaState * state); 

// metatable
#define TYPE_METATABLE_NAME(val) ("_MT" + to_string(val.type))

#endif