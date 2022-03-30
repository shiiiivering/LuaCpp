#ifndef LUA_STATE_H
#define LUA_STATE_H

#include "lua_stack.h"
#include "lua_table.h"
#include "lua_closure.h"
#include "common/lua.h"

class luaVm;
class luaStack;
class luaState{
    public:
    luaStack * stack;    
    TValue registry; // registry table. For global variables.
    luaVm * vm;
    public:
    luaState(luaVm * vm);
    void printStack();

    // basic stack operation
    int GetTop();
    int AbsIndex(int idx); // do not check validation of index
    bool CheckStack(int n){stack->check(n); return true;}; // do not implement now
    void Pop(int n);
    void Copy(int src, int tar);

    void PushValue(int idx); // get value in idx and push it to top of the stack
    void Replace(int idx); // pop stack and replace it to idx
    void Insert(int idx); // pop stack top, insert to idx, and move the rest
                          // elements. Special version of rotate
    void Remove(int idx); // delete value in idx and move down uppervalues
    void Rotate(int idx, int n); // implement by reverse. **LOW EFFICIENTY**
    void SetTop(int idx); // set top. and do pop or push to adjust the stack.
                          // idx here is relevant index
    Instruction GetCurrentInstruction(){return stack->closure->proto->Code[stack->pc];}
    TValue GetConstant(int idx){return stack->closure->proto->Constants[idx];}

    // push operations
    void PushNil();
    void PushBoolean(bool b);
    void PushInteger(lua_Integer i);
    void PushNumber(lua_Number n);
    void PushString(const string & s);
    void Push(const TValue & v);

    // access operations
    string TypeName(int type);
    int Type(int idx);
    bool IsNone(int idx){return Type(idx) == LUA_TNONE;}
    bool IsNil(int idx){return Type(idx) == LUA_TNIL;}
    bool IsNoneOrNil(int idx){return Type(idx) <= LUA_TNIL;}
    bool IsBoolean(int idx){return Type(idx) == LUA_TBOOLEAN;}
    // some type cast involved
    bool IsString(int idx){
        // is string or number
        int t = Type(idx);
        return t == LUA_SHRSTR || t == LUA_LNGSTR || t == LUA_NUMINT || t == LUA_NUMFLT ;
    }
    bool IsNumber(int idx); // consider type cast
    bool IsInteger(int idx); // not consider type cast
    
    
    bool ToBoolean(int idx);
    bool convertToBoolean(const TValue & v);
    lua_Number ToNumberX(int idx, int * status = nullptr);
    lua_Number ToNumber(int idx);
    lua_Integer ToIntegerX(int idx, int * status = nullptr);
    lua_Integer ToInteger(int idx);
    string ToStringX(int idx, int * status = nullptr);
    string ToString(int idx);

    // methods for base operations
    void Arith(ArithOp op);
    bool Compare(int idx1, int idx2, CompareOp op);
    void Len(int idx);
    void Concat(int n);

    // Table methods
    // get
    void NewTable();
    void CreateTable(int nArr, int nRec);
    int GetTable(int idx);
    int getTable(const TValue & t, const TValue & k, bool raw);
    int GetField(int idx, const string & k);
    int GetI(int idx, lua_Integer i);
    // set
    void SetTable(int idx);
    void setTable(const TValue & t, const TValue & k, const TValue & v, bool raw);
    void SetField(int idx, const string & k);
    void SetI(int idx, lua_Integer k);
    // print table
    void printTable(TValue v);
    // iterator
    bool Next(int idx);

    // FUNCTION CALL
    void pushLuaStack(luaStack * new_stack);
    void popLuaStack();
    int Load(string path, string name, string mode);
    void Call(int nArgs, int nResults);
    void callLuaClosure(int nArgs, int nResults, TValue & val);
    void pushLuaClosure(int nArgs, TValue & val);
    void popLuaClosure(int nResults, TValue & val);
    void runLuaClosure();

    // CPP FUNCTION
    void PushCppFunction(CppFunction f);
    bool IsCppFunction(int idx);
    CppFunction ToCppFunction(int idx);
    void callCppClosure(int nArgs, int nResults, TValue & val);
    void PushCppClosure(CppFunction f, int n);
    int LuaUpvalueIndex(int i){return LUA_REGISTRYINDEX - i;}

    // Global Function
    void PushGlobalTable();
    int GetGlobal(const TValue & name);
    void SetGlobal(const TValue & name);
    void Register(const TValue & name, CppFunction f);

    // metatable
    void setMetatable(TValue val, TValue mt);
    TValue getMetatable(const TValue & val);
    TValue callMetamethod(TValue & a, TValue & b, const string & metamethodName, int * status = nullptr);
    TValue getMetafield(const TValue & a, const TValue & fieldName);
    bool hasMetafield(const TValue & a, const TValue & key);

    bool GetMetatable(int idx);
    void SetMetatable(int idx);
    int RawLen(int idx);
    bool RawEqual(int idx1, int idx2);
    int RawGet(int idx);
    void RawSet(int idx);
    int RawGetI (int idx, lua_Integer i);
    void RawSetI(int idx, lua_Integer i);
};



#endif