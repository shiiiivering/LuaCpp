#ifndef BINARY_CHUNK
#define BINARY_CHUNK
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "vm/instruction.h"
#include "../common/lua_object.h"
#include "../common/lua.h"
using namespace std;

//macro of chunk header
#define LUA_SIGNATURE       "\x1bLua"
#define LUAC_VERSION        0x53
#define LUAC_FORMAT         0
#define LUAC_DATA           "\x19\x93\r\n\x1a\n"
#define CINT_SIZE           4
#define CSIZE_SIZE          8
#define INSTRUCTION_SIZE    4
#define LUA_INTEGER_SIZE    8
#define LUA_NUMBER_SIZE     8
#define LUAC_INT            0x5678
#define LUAC_NUM            370.5

// TAG VALUE
#define TAG_NIL             0x00
#define TAG_BOOLEAN         0x01
#define TAG_NUMBER          0x03
#define TAG_INTEGER         0x13
#define TAG_SHORT_STR       0x04
#define TAG_LONG_STR        0x14

typedef struct header{
    char signature[4];      // signature, also called Magic Number
                            // if binary chunk not begin with \x1bLua 0x1B4C7561, 
                            // then refuse to load
    char version;           // Version number of lua, if not match, then lua vm will
                            // refuse to load
    char format;            // Format number, will be 0 in official implementation
    char luacData[6];       // 0x19930D0A, advance verification

                            // check byte num of some data type. refuse to load if not match
    char cintSize;          // size of cint
    char sizetSize;         // size of size_t
    char instructionSize;   // size of Lua VM instruction   
    char luaIntegerSize;    // Lua integer
    char luaNumberSize;     // Lua Number(float)

    int64_t luacInt;      // lua integer number 0x5678
                            // to check big-endian or little-endian of binary chunk
    double luacNum;         // lua float number 370.5
                            // check float number format of binary chunk
}header;

typedef struct Upvalue{
    char Instack;
    char Idx;
}Upvalue;

class LocVar{
    public:
    string VarName;
    unsigned int StartPC;
    unsigned int EndPC;
    LocVar(){};
    LocVar(const string &var_name, unsigned int StartPC, unsigned int EndPC)
        : VarName(var_name), StartPC(StartPC), EndPC(EndPC){};
};

// typedef union Value{
//     string * str;
//     int64_t i;
//     double n;
//     bool b;
// }Value;

// class TValue{
//     public:
//     Value v_;
//     char tt_;
//     ~TValue(){if(tt_ == TAG_SHORT_STR || tt_ == TAG_LONG_STR) delete v_.str;}
// };

//prototype
class Prototype{
    public:
    string                  Source;             // source func, may begin with @ or = or other
                                                // @: this chunk is compiled from lua source file
                                                // =: compiled from stdin if "=stdin"

    uint32_t                LineDefined;        // begin line number, zero if and only if main func
    uint32_t                LastLineDefined;    // end line number, zero if and only if main func
    char                    NumParams;          // **fixed** parameter number
    char                    IsVararg;           // if has variable number of arguments
    char                    MaxStackSize;       // reg num (virtual reg of vm)(stack organization)
    vector<uint32_t>        Code;               // instruction table (4 bytes per instruction)
    vector<TValue>          Constants;          // literals in lua program
                                                // nil(not stored)
                                                // boolean (byte 0, 1)
                                                // integer (Lua Integer)
                                                // number (Lua Number)
                                                // string (short, long)
    vector<Upvalue>         Upvalues;    
    vector<Prototype *>      Protos;          // sub-function prototype

                                                // these three tables store the debug info
    vector<uint32_t>    LineInfo;           // line number info. relate each instruction to line
                                                // number of source code
    vector<LocVar>          LocVars;            // local variables
    vector<string>          UpvalueNames;       // upvalue names, one-to-one correlate with Upvalues
};

class ChunkReader{
    private:
    ifstream chunkFile;

    public:

    ChunkReader(const string & file_name){
        bool status = reader_init(file_name);
    }
    bool reader_init(const string & file_name);
    ~ChunkReader();

    char readByte();
    uint32_t readUint32();
    uint64_t readUint64();
    int64_t readLuaInteger();
    double readLuaNumber();
    char * readBytes(int size);
    int readBytes(char * addr, int size);
    string readString();
};

class binaryChunk{
    header head;
    char byte;
    Prototype *mainFunc;
    ChunkReader reader;

    public:
    bool checkHeader();
    binaryChunk(string file_name);

    Prototype * Undump();
    vector<uint32_t> readCode();
    void readConstant(TValue * constant);
    vector<TValue> readConstants();
    vector<Upvalue> readUpvalues();
    vector<Prototype *> readProtos(const string & parent_source);
    vector<uint32_t> readLineInfo();
    vector<LocVar> readLocVars();
    vector<string> readUpvalueNames();
    Prototype * readProto(const string & parent_source);

    // print chunk info
    void printHeader(Prototype * f);
    void printOperands(Instruction & ins);
    void printCode(Prototype * f);
    void printDetail(Prototype * f);
    void List(Prototype *f);
    void printList();
};

#endif