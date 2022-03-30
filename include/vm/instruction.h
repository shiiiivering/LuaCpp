#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "vector"
#include "opcodes.h"
#include "string"
using namespace std;
extern const Instr opcodes[];

#define MAXARG_Bx       ((1 << 18) - 1)
#define MAXARG_sBx      (MAXARG_Bx >> 1)
class Instruction {
    private:
    uint32_t opcode;    // it is unsigned number here, so ">>" operation will 
                        // fill the new leftmost bit with 0

    public:
    Instruction(): opcode(0){}
    Instruction(const uint32_t c): opcode(c){}
    Instruction(const Instruction & ins): opcode(ins.opcode){}
    int Opcode(){
        return int (opcode & 0x3F);
    }

    vector<int >ABC(){
        // return {a, b, c}
        // B and C may not be used in some instructions
        // if the highest bit of B and C is 1, then it means the operand will be
        // searched in **CONSTANT VALUE TABLE**, and output in negative number
        return {
            int((opcode >> 6) & 0xFF),
            int((opcode >> 23) & 0x1FF),
            int((opcode >> 14) & 0x1FF)
        };
    }
    vector<int >ABx(){
        // return {a, bx}
        return {
            int((opcode >> 6) & 0xFF),
            int(opcode >> 14),
        };
    }
    vector<int >AsBx(){
        // return {a, sBx}
        // sBx is signed integer with 18bits
        // lua vm uses **Offset Binary (also called Excess-K)**
        // if sBx's value is x when interpreted as unsigned int,
        // then the value becomes x - K when interpreted as signed.
        // Where K means half of the largest integer sBx can 
        // represent. Which is MAXARG_sBx
        return {
            int((opcode >> 6) & 0xFF),
            int(((int)(opcode >> 14)) - MAXARG_sBx),
        };
    }
    int Ax(){
        return int(opcode >> 6);
    }

    string GetOpName(){
        return opcodes[Opcode()].name;
    }
    char GetOpMode(){
        return opcodes[Opcode()].opMode;
    }
    char GetBMode(){
        return opcodes[Opcode()].argBMode;
    }
    char GetCMode(){
        return opcodes[Opcode()].argCMode;
    }
    uint32_t GetOpCode(){
        return opcode;
    }
};


#endif