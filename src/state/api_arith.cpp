#include "api_arith.h"
#include "lua_state.h"
#include <string>
using namespace std;

// implementing function Arith

luaOperator luaOperators[] = {
    luaOperator{"__add", iadd, fadd},        // +
    luaOperator{"__sub", isub, fsub},        // -
    luaOperator{"__mul", imul, fmul},        // *
    luaOperator{"__mod", imod, lua_fmod},    // %
    luaOperator{"__pow", nullptr, lua_pow},  // pow
    luaOperator{"__div", nullptr, lua_div},  // /
    luaOperator{"__idiv", iidiv, fidiv},      // //
    luaOperator{"__band", band, nullptr},     // &
    luaOperator{"__bor", bor, nullptr},      // |
    luaOperator{"__bxor", bxor, nullptr},     // ^
    luaOperator{"__shl", shl, nullptr},      // <<
    luaOperator{"__shr", shr, nullptr},      // >>
    luaOperator{"__unm", iunm, nullptr},     // - (unary)
    luaOperator{"__bnot", bnot, nullptr}      // ~ 
};

TValue _arith(TValue & a, TValue & b, luaOperator & op){
    int status = LUA_OK;
    if(op.floatFunc == nullptr){ // bitwise operator
        lua_Integer x = a.convertToInteger(&status);
        if(status == LUA_OK){
            lua_Integer y = b.convertToInteger(&status);
            if(status == LUA_OK){
                return TValue(op.integerFunc(x, y));
            }
        }
    }
    else{ // arith
        if(op.integerFunc != nullptr){
            if(a.type == LUA_NUMINT && b.type == LUA_NUMINT){ 
                // arith operator. Perform integer operations 
                // only when both of the operands are integer. 
                return TValue(op.integerFunc(a.value.i, b.value.i));
            }
        }
        lua_Number x = a.convertToFloat(&status);
        if(status == LUA_OK){
            lua_Number y = b.convertToFloat(&status);
            if(status == LUA_OK){
                return TValue(op.floatFunc(x, y));
            }
        }
    }
    return TValue();
}

void luaState::Arith(ArithOp op){
    TValue b(stack->top());
    stack->pop();
    TValue a;
    if(op != LUA_OPUNM && op != LUA_OPBNOT){
        a = stack->top();
        stack->pop();
    }
    else{
        a = b;
    }
    TValue result = _arith(a, b, luaOperators[op]);
    if(result.type != LUA_TNIL){
        stack->push(result);
        return;
    }

    string metaname = luaOperators[op].metamethod;
    int status = LUA_OK;
    result = callMetamethod(a, b, metaname, &status);
    if(status == LUA_OK){
        stack->push(result);
        return;
    }
    panic("arithmetic error !!!");
}
