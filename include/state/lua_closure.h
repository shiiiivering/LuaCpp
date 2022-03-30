#ifndef CLOSURE_H
#define CLOSURE_H

#include "../binchunk/binary_chunk.h"
#include <memory>
#include <vector>
using namespace std;

class upvalue {
    private:
    TValue * val;
    bool open;
    public :

    TValue * get(){
        return val;
    }

    void set(TValue * v){
        if(open == false && v != val){
            delete val;
        }
        val = v;
        open = false;
        return ;
    };
    upvalue(TValue * v) : val(v), open(true){}
    upvalue() = delete;
    ~upvalue(){
        if(!open){ // if an upvalue is closed, then free it;
            delete val;
            val = nullptr;
        }
    }
};

class luaClosure{
    public:
    Prototype *  proto;     // lua closure
    CppFunction cppFunc;  // cpp closure
    vector<shared_ptr<upvalue>> upvals; // upvalues. 
    vector<TValue> cppFuncUpvalCache;
    int ref;

    public:
    luaClosure(Prototype * proto);
    luaClosure(CppFunction f);
    luaClosure(CppFunction f, int nUpvals);
};

#endif