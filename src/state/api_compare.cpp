#include "lua_state.h"
#include "common/lua_object.h"
// Compare operation. gets two operands from stack and compare them. 
bool _eq(TValue & a, TValue & b, luaState * ls){
    switch(a.type){
        case LUA_TNIL:
            return b.type == LUA_TNIL;
        case LUA_TTABLE:
            if(b.type == LUA_TTABLE && a.value.t != b.value.t && ls){
                int status = LUA_OK;
                TValue result = ls->callMetamethod(a, b, "__eq");
                if(status == LUA_OK){
                    return ls->convertToBoolean(result);
                }
            }
            return a.type == b.type && a.value.t == b.value.t;
        case LUA_TBOOLEAN:
            return (b.type == LUA_TBOOLEAN && b.value.b == a.value.b);
        case LUA_LNGSTR:
        case LUA_SHRSTR:
            return ((b.type == LUA_LNGSTR || b.type == LUA_SHRSTR) 
            && *(a.value.str) == *(b.value.str));
        case LUA_NUMINT:
            if(b.type == LUA_NUMINT) return b.value.i == a.value.i;
            else if(b.type == LUA_NUMFLT) return FLOAT_INTEGER_EQUAL(b.value.n, a.value.i);
            else return false;
        case LUA_NUMFLT:
            if(b.type == LUA_NUMINT)return FLOAT_INTEGER_EQUAL(b.value.i, a.value.n);
            else if(b.type == LUA_NUMFLT)return b.value.n == a.value.n; // WARNING!! Comparation between floats is useless in most times here
            else return false;
        default:
            return &a == &b; // unresolvable comparation, just return if they are the same value
    }
}

bool _lt(TValue & a, TValue & b, luaState * ls){
    switch(a.type){
        case LUA_SHRSTR:
        case LUA_LNGSTR:
            if (b.type == LUA_SHRSTR || b.type == LUA_LNGSTR) return *(a.value.str) < *(b.value.str);
            else break;
        case LUA_NUMINT:
            if(b.type == LUA_NUMINT) return a.value.i < b.value.i;
            else if(b.type == LUA_NUMFLT) return a.value.i < b.value.n;
            else break;
        case LUA_NUMFLT:
            if(b.type == LUA_NUMINT) return a.value.n < b.value.i;
            else if(b.type == LUA_NUMFLT) return a.value.n < b.value.n;
            else break;
    }
    int status = LUA_OK;
    TValue result = ls->callMetamethod(a, b, "__lt", &status);
    if(status == LUA_OK){
        return ls->convertToBoolean(result);
    }
    panic("comparison error!!!");
}

bool _le(TValue & a, TValue & b, luaState * ls){
    switch(a.type){
        case LUA_SHRSTR:
        case LUA_LNGSTR:
            if (b.type == LUA_SHRSTR || b.type == LUA_LNGSTR) return *(a.value.str) <= *(b.value.str);
            else break;
        case LUA_NUMINT:
            if(b.type == LUA_NUMINT) return a.value.i <= b.value.i;
            else if(b.type == LUA_NUMFLT) return a.value.i <= b.value.n || FLOAT_INTEGER_EQUAL(a.value.i, b.value.n);
            else break;
        case LUA_NUMFLT:
            if(b.type == LUA_NUMINT) return a.value.n < b.value.i || FLOAT_INTEGER_EQUAL(a.value.n, b.value.i);
            else if(b.type == LUA_NUMFLT) return a.value.n <= b.value.n;
            else break;
    }
    int status = LUA_OK;
    TValue result = ls->callMetamethod(a, b, "__le", &status);
    if(status == LUA_OK){
        return ls->convertToBoolean(result);
    }else{
        result = ls->callMetamethod(b, a, "__lt", &status);
        if(status == LUA_OK){
            return !ls->convertToBoolean(result);
        }
    }
    panic("comparison error!!!");
}



bool luaState::Compare(int idx1, int idx2, CompareOp op){
    if(!stack->isValid(idx1) || !stack->isValid(idx2)){
        return false;
    }
    TValue a = stack->get(idx1);
    TValue b = stack->get(idx2);
    switch(op){
        case LUA_OPEQ: return _eq(a, b, this);
        case LUA_OPLT: return _lt(a, b, this);
        case LUA_OPLE: return _le(a, b, this);
        default: panic("invalid compare operation!");
    }
}

bool luaState::RawEqual(int idx1, int idx2){
    if(!stack->isValid(idx1) || !stack->isValid(idx2)){
        return false;
    }
    TValue a = stack->get(idx1);
    TValue b = stack->get(idx2);
    return _eq(a, b, nullptr);
}