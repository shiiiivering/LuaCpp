#ifndef LUA_VM_H
#define LUA_VM_H
#include "../state/lua_state.h"
#include "../state/lua_closure.h"
#include "../binchunk/binary_chunk.h"
#include "instruction.h"
#include "../common/lua.h"

/***************************************
 * IMPORTANT
 * index in instructions begins with 0,
 * but in luaAPI, index begins with 1.
 * So, the register index plus 1 means the
 * relative stack index
*****************************************/

#define BINARY_ALG(func_name,op_name)  \
void func_name (Instruction i){         \
    _binaryArith(i, op_name);           \
}                                       

#define UNARY_ALG(func_name,op_name)   \
void func_name (Instruction i){         \
    _unaryArith(i, op_name);            \
}

#define COMPARE_ALG(func_name,op_name) \
void func_name (Instruction i){         \
    _compare(i, op_name);               \
}

class luaVm{
    private:
    // int pc;            // program counter
    // Prototype * proto; // function prototype to get instruction and constant value

    // TODO: now is public, for function test
    public:
    luaState state;
    public:
    luaVm():state(this){};

    // vm interfaces
    int PC();                   // return current PC(test only)
    void AddPC(int n);          // modify PC(for JMP)
    Instruction Fetch();        // fetch current instruction
    void GetConst(int idx);     // push const to stack
    void GetRK(int rk);         // push const or stack value to stack

    // vm operations
    // misc operations
    void move(Instruction inst);
    void jmp(Instruction inst);

    // load operations
    void loadNil(Instruction inst);
    void loadBool(Instruction inst);
    void loadK(Instruction inst);
    void loadKx(Instruction inst);

    // algrithm operations
    // binary
    void _binaryArith(Instruction inst, ArithOp op);
    BINARY_ALG(add, LUA_OPADD)
    BINARY_ALG(sub, LUA_OPSUB)
    BINARY_ALG(mul, LUA_OPMUL)
    BINARY_ALG(mod, LUA_OPMOD)
    BINARY_ALG(pow, LUA_OPPOW)
    BINARY_ALG(div, LUA_OPDIV)
    BINARY_ALG(idiv, LUA_OPIDIV)
    BINARY_ALG(band, LUA_OPBAND)
    BINARY_ALG(bor, LUA_OPBOR)
    BINARY_ALG(bxor, LUA_OPBXOR)
    BINARY_ALG(shl, LUA_OPSHL)
    BINARY_ALG(shr, LUA_OPSHR)
    // unary
    void _unaryArith(Instruction inst, ArithOp op);
    UNARY_ALG(unm, LUA_OPUNM)
    UNARY_ALG(bnot, LUA_OPBNOT)

    // string related instructions
    void _len(Instruction inst);
    void concat(Instruction inst);

    // compare operations
    void _compare(Instruction inst, CompareOp op);
    COMPARE_ALG(eq, LUA_OPEQ)
    COMPARE_ALG(lt, LUA_OPLT)
    COMPARE_ALG(le, LUA_OPLE)

    // logic operations
    void vm_not(Instruction inst);
    void testSet(Instruction inst);
    void test(Instruction inst);

    // for loop
    void forPrep(Instruction inst);
    void forLoop(Instruction inst);

    // table instruction
    void newTable(Instruction inst);
    void getTable(Instruction inst);
    void setTable(Instruction inst);
    void setList(Instruction inst);
    void tForCall(Instruction inst);
    void tForLoop(Instruction inst);

    // closure and function call instruction
    void closure(Instruction inst);
    void call(Instruction inst);
    int PushFuncAndArgs(int a, int b);
    void PopResults(int a, int c);
    void FixStack(int a);
    void vm_return(Instruction inst);
    void vararg(Instruction inst);
    void tailcall(Instruction inst);
    void self(Instruction inst);

    // upvalue
    void getUpvalue(Instruction inst);
    void setUpvalue(Instruction inst);
    void getTabUp(Instruction inst);
    void setTabUp(Instruction inst);
    void CloseUpvalues(int a);

    
    int RegisterCount();
    void LoadVararg(int n);
    void LoadProto(int idx);
    // execute an instruction
    void Call(int nArgs, int nResults);
    void callLuaClosure(int nArgs, int nResults, TValue & val);
    void callCppClosure(int nArgs, int nResults, TValue & val);
    void runLuaClosure();
    void Execute(Instruction inst);

    // // global inst
    // void getTabUp(Instruction inst);
};


#endif