#include "lua_closure.h"
luaClosure::luaClosure(Prototype * proto): ref(0), proto(proto), cppFunc(nullptr){
    int nUpvals = proto->Upvalues.size();
    if(nUpvals > 0){
        upvals.resize(nUpvals); // allocate space for upvalues. Num of upvalues
                                // is known when compile time, because Lua is a 
                                // static Scoping language
    }
}

luaClosure::luaClosure(CppFunction f): ref(0), cppFunc(f), proto(nullptr){}

luaClosure::luaClosure(CppFunction f, int nUpvals): ref(0), cppFunc(f), proto(nullptr){
    if(nUpvals > 0){
        upvals.resize(nUpvals);
    }
}