#include <iostream>
#include <stack>
#include <string>
#include "binchunk/binary_chunk.h"
#include "vm/opcodes.h"
#include "vm/lua_vm.h"
#include "state/lua_state.h"
#include <string.h>
#include <memory>
#include <cmath>
#include "state/lua_state.h"
using namespace std;

class A{
    public:
    int a;
    void print(){
        cout << "a = " << a << endl;
    }
};
auto func_p = &A::print;


int lua_print(luaState * ls){
    int nArgs = ls->GetTop();
    for(int i = 1; i <= nArgs; i++){
        if (ls->IsBoolean(i)){
            printf("%s", (ls->ToBoolean(i) ? "true": "false"));
        }
        else if(ls->IsString(i)){
            cout << ls->ToString(i);
        }
        else{
            cout << ls->TypeName(ls->Type(i));
        }
        if(i < nArgs){
            cout << "\t";
        }
    }
    cout << endl;
    return 0;
}

int getMetatable(luaState * ls){
    if(!ls->GetMetatable(1)){
        ls->PushNil();
    }
    return 1;
}

int setMetatable(luaState * ls){
    ls->SetMetatable(1);
    return 1;
}

int next(luaState * ls){
    ls->SetTop(2);
    if(ls->Next(1)){
        return 2;
    }
    else{
        ls->PushNil();
        return 1;
    }
}

int pairs(luaState * ls){
    ls->PushCppFunction(next);
    ls->PushValue(1);
    ls->PushNil();
    return 3;
}

int _iPairAux(luaState * ls){
    lua_Integer i = ls->ToInteger(2) + 1;
    ls->PushInteger(i);
    if(ls->GetI(1, i) == LUA_TNIL){
        return 1;
    }
    else{
        return 2;
    }
}

int iPairs(luaState * ls){
    ls->PushCppFunction(_iPairAux);
    ls->PushValue(1);
    ls->PushInteger((lua_Integer)0);
    return 3;
}



int main(int argc, char ** argv){
    if(argc < 2){
        cout << "need binary file path" << endl;
        return 0;
    }

    luaVm vm;
    // vm.state.Load("/home/zhenggei/code/lua_cpp/lua/luac.out", "luac", "b");
    vm.state.Load(argv[1], (argc >= 3 ? argv[2] : "luac"), "b");
    vm.state.Register(TValue(string("print")), lua_print);
    vm.state.Register(TValue(string("getmetatable")), getMetatable);
    vm.state.Register(TValue(string("setmetatable")), setMetatable);
    vm.state.Register(TValue(string("next")), next);
    vm.state.Register(TValue(string("pairs")), pairs);
    vm.state.Register(TValue(string("ipairs")), iPairs);
    vm.Call(0, 0);
}