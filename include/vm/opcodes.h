#ifndef OPCODES_H
#define OPCODES_H
// #include "lua_vm.h"
// #include "instruction.h"

enum OpCode {
	OP_MOVE,        // A, B; R[A] = R[B] load a value from a register to another register;
	OP_LOADK,       // A, B; R[A] = RK[B]; load a value from k to register
    OP_LOADKX,
    OP_LOADBOOL,    // A B C   R(A) := (Bool)B; if(C) PC++;
	OP_LOADNIL,     // A B     R(A) := ... := R(B) := nil

    OP_GETUPVAL,    // A, B; R[A] = UpValue[B]
    OP_GETTABUP,   // A, B, C; R[A] = Upval[B][RK[C]]
	OP_GETTABLE,    // A, B, C; R[A] = R[B][RK[C]]

    OP_SETTABUP,    // A B C   UpValue[A][RK(B)] := RK(C)
    OP_SETUPVAL,    // A B     UpValue[B] := R(A)
	OP_SETTABLE,    // A B C   R(A)[RK(B)] = RK(C)

    OP_NEWTABLE,    // A B C   R(A) := {} (size = B,C)

	OP_SELF,        // A, B, C; R(A+1) = R(B); R(A) = R(B)[RK(C)];

    OP_ADD,			// A B C   R(A) := RK(B) + RK(C)
	OP_SUB,			// A B C   R(A) := RK(B) - RK(C)
	OP_MUL,			// A B C   R(A) := RK(B) * RK(C)
    OP_MOD,			// A B C   R(A) := RK(B) % RK(C)
    OP_POW,			// A B C   R(A) := RK(B) ^ RK(C)
	OP_DIV,			// A B C   R(A) := RK(B) / RK(C)
	OP_IDIV,		// A B C   R(A) := RK(B) // RK(C)
	
	OP_BAND,		// A B C   R(A) := RK(B) & RK(C)
	OP_BOR,			// A B C   R(A) := RK(B) | RK(C)
	OP_BXOR,		// A B C   R(A) := RK(B) ~ RK(C)
	OP_SHL,			// A B C   R(A) := RK(B) << RK(C)
	OP_SHR,			// A B C   R(A) := RK(B) >> RK(C)

    OP_UNM,			// A B     R(A) := -R(B)
    OP_BNOT,		// A B     R(A) := ~R(B)
	OP_NOT,			// A B     R(A) := not R(B)
    OP_LEN,			// A B     R(A) := length of R(B)

	OP_CONCAT,		// A B C   R(A) := R(B).. ... ..R(C)

    OP_JMP,		// A sBx   pc+=sBx; if (A) close all upvalues >= R(A - 1)

    OP_EQ,			// A B C   if ((RK(B) == RK(C)) ~= A) then pc++
	OP_LT,			// A B C   if ((RK(B) <  RK(C)) ~= A) then pc++
	OP_LE,			// A B C   if ((RK(B) <= RK(C)) ~= A) then pc++

    OP_TEST,		// A, B, C; if not (R(A) <=> C) then pc++];
	OP_TESTSET,		// A B C   if (R(B) <=> C) then R(A) := R(B) else pc++ 

	OP_CALL,        // A, B, C; R[A], ... R[A + C - 2] = R(A)(R[A + 1], ... , R[A + B - 1])
					// A index the function in the stack. 
					// B represents the number of params, if B is 1, the function has no parameters, 
                    // else if B is greater than 1, the function has B - 1 parameters, and if B is 0
					// it means that the function parameters range from A+1 to the top of stack
					// C represents the number of return, if C is 1, there is no value return, else if C is greater than 1, then it has C - 1 return values, or if C is 0
					// the return values range from A to the top of stack
    OP_TAILCALL,
	OP_RETURN,      // A, B; return R[A], ... R[A + B - 2]
					// return the values to the calling function, B represent the number of results. if B is 1, that means no value return, if B is greater than 1, it means 
					// there are B - 1 values return. And finally, if B is 0, the set of values range from R[A] to the top of stack, are return to the calling function
    OP_FORLOOP,     // A sBx   FORLOOP A sBx R(A) += R(A+2)
					//         if R(A) < ? = R(A + 1) then {
					//	          PC += sBx; R(A + 3) = R(A)
					//         }
    OP_FORPREP,     // A sBx   FORPREP A sBx R(A) -= R(A+2); PC += sBx
    OP_TFORCALL,    // A C	R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));
    OP_TFORLOOP,    // A sBx	if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }

	OP_SETLIST,     // A B C   R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B
	
	OP_CLOSURE,     // A Bx	R(A) := closure(KPROTO[Bx])
    OP_VARARG,
    OP_EXTRAARG,
	NUM_OPCODES,
};

// The whole instruction is 32 bits
// opcode is 6 bits, the description of instructions are as follow
/***
+--------+---------+---------+--------+--------+
|  iABC  |    B:9  |   C:9   |   A:8  |Opcode:6|
+--------+---------+---------+--------+--------+
|  iABx  |   Bx:18(unsigned) |   A:8  |Opcode:6|
+--------+---------+---------+--------+--------+
|  iAsBx |   sBx:18(signed)  |   A:8  |Opcode:6|
+--------+---------+---------+--------+--------+
***/
enum OpMode {
	IABC,
	IABx,
	IAsBx,
    IAx
};

enum OpArgMask {
    OpArgN,     // argument is not used
	OpArgU,     // argument is used, and it's value is in instruction
	OpArgR,     // argument is used, and it's value is register, or a jump offset
	OpArgK,     // argument is used, and it's value is in constant table, or register/constant
};
class Instruction;
class luaVm;
typedef struct Instr {
    char testFlag;
    char setAFlag;
    char argBMode;
    char argCMode;
    char opMode;
    char name[10];
	void (luaVm::*action)(Instruction instr);
}Instr;

extern const Instr opcodes[NUM_OPCODES];

#endif