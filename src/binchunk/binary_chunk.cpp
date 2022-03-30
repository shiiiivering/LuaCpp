#include "binary_chunk.h"
#include "vm/instruction.h"
#include "common/lua.h"
#include <string>
#include <iostream>
#include <exception>
using namespace std;
bool ChunkReader::reader_init(const string & file_name){
    if(chunkFile.is_open()){
        cout << "chunk file has already been opened!" << endl;
        return false;
    }
    else{
        try{
            chunkFile.open(file_name, std::ios::binary);
        }catch(...){// TODO resolve the exception type
            cout << "open chunk file failed!" << endl;
            return false;
        }
        return true;
    }
}

char ChunkReader::readByte(){
    char ret;
    chunkFile.read((char *)&ret, sizeof(char));
    return ret;
}

uint32_t ChunkReader::readUint32(){
    char ret[4];
    chunkFile.read((char *)&ret, sizeof(uint32_t));
    return *((uint32_t*)(ret));
}
uint64_t ChunkReader::readUint64(){
    char ret[8];
    chunkFile.read((char *)&ret, sizeof(uint64_t));
    return *((uint64_t*)(ret));
}

int64_t ChunkReader::readLuaInteger(){
    return int64_t(readUint64());
}

double ChunkReader::readLuaNumber(){
    uint64_t r = readUint64();
    double *p = (double *)(&r);
    return *p;
}

char * ChunkReader::readBytes(int size){
    // WARNING!! if no address passed in, this function will allocate new memory, which must be freed manually
    char * c = new char[size];
    chunkFile.read(c, size);
    return c;
}

int ChunkReader::readBytes(char * addr, int size){
    if(addr == nullptr){
        return -1;
    }
    chunkFile.read(addr, size);
    return size;
}

string ChunkReader::readString(){
    int size = uint32_t(readByte());
    if(size == 0){
        return "";
    }
    if (size == 0xFF){ // long string
        size = uint32_t(readUint64());
    }
    char * bytes = readBytes(size - 1);
    string ret = string(bytes, bytes + size - 1);
    delete bytes;
    return ret;
}

ChunkReader::~ChunkReader(){
    chunkFile.close();
}




bool binaryChunk::checkHeader(){
    char * bytes = new char[10];
    reader.readBytes(bytes, 4);
    if (string(bytes, bytes + 4) != LUA_SIGNATURE){
        panic("not a precompiled chunk");
        return false;
    }
    if(reader.readByte() != LUAC_VERSION){
        panic("not a precompiled chunk");
        return false;
    }
    if(reader.readByte() != LUAC_FORMAT){
        panic("format mismatch");
        return false;
    }
    reader.readBytes(bytes, 6);
    if(string(bytes, bytes + 6) != LUAC_DATA){
        panic("corrupted");
        return false;
    }
    if(reader.readByte() != CINT_SIZE){
        panic("int size mismatch");
        return false;
    }
    if(reader.readByte() != CSIZE_SIZE){
        panic("size_t size mismatch");
        return false;
    }
    if(reader.readByte() != INSTRUCTION_SIZE){
        panic("instruction size mismatch");
        return false;
    }
    if(reader.readByte() != LUA_INTEGER_SIZE){
        panic("lua_Integer size mismatch");
        return false;
    }
    if(reader.readByte() != LUA_NUMBER_SIZE){
        panic("lua_Number size mismatch");
        return false;
    }
    if(reader.readLuaInteger() != LUAC_INT){
        panic("endianness mismatch");
        return false;
    }
    float temp = reader.readLuaNumber();
    if(temp != LUAC_NUM){
        panic("float format mismatch");
        return false;
    }
    return true;
}

binaryChunk::binaryChunk(string file_name) : reader(file_name){
    mainFunc = nullptr;
}

Prototype * binaryChunk::Undump (){
    checkHeader();
    reader.readByte();
    return readProto("");
};

vector<uint32_t> binaryChunk::readCode(){
    int size = reader.readUint32();
    vector<uint32_t> code(size, 0);
    //return code;
    for(int i = 0; i < code.size(); i++){
        code[i] = reader.readUint32();
    }
    return code;
}

void binaryChunk::readConstant(TValue * constant){
    constant->type = reader.readByte();
    switch (constant->type){
        case TAG_NIL:{
            return;
        }
        case TAG_BOOLEAN: {
            constant->value.b = reader.readByte() != 0;
            return;
        }
        case TAG_INTEGER: {
            constant->value.i = reader.readLuaInteger();
            return;
        }
        case TAG_NUMBER: {
            constant->value.n = reader.readLuaNumber();
            return;
        }
        case TAG_SHORT_STR: {
            string temp = reader.readString();
            constant->value.str = new string;
            *(constant->value.str) = temp;
            return;
        }
        case TAG_LONG_STR: {
            string temp = reader.readString();
            constant->value.str = new string;
            *(constant->value.str) = temp;
            return;
        }
        default:{
            panic("corrupted!");
            return;
        }
    }
}

vector<TValue> binaryChunk::readConstants(){
    int size = reader.readUint32();
    vector<TValue> constants(size);
    for (int i = 0; i < constants.size(); i++){
        readConstant(&(constants[i]));
    }
    return constants;
}
vector<Upvalue> binaryChunk::readUpvalues(){
    int size = reader.readUint32();
    vector<Upvalue> upvalues(size);
    for(int i = 0; i < upvalues.size(); i++){
        upvalues[i].Instack = reader.readByte();
        upvalues[i].Idx = reader.readByte();
    }
    return upvalues;
}

vector<Prototype *> binaryChunk::readProtos(const string& parent_source){
    int size = reader.readUint32();
    vector<Prototype *> prototypes((int)size);
    for(int i = 0; i < prototypes.size(); i++){
        //return prototypes;
        prototypes[i] = readProto(parent_source);
    }
    return prototypes;
}
vector<uint32_t> binaryChunk::readLineInfo(){
    int size = reader.readUint32();
    vector<uint32_t> lineInfo(size);
    for(int i = 0; i < lineInfo.size(); i++){
        lineInfo[i] = reader.readUint32();
    }
    return lineInfo;
}

vector<LocVar> binaryChunk::readLocVars(){
    int size = reader.readUint32();
    vector<LocVar> loc_vars;
    loc_vars.reserve(size);
    for(int i = 0; i < size; i++){
        string str = reader.readString();
        uint32_t start = reader.readUint32();
        uint32_t end = reader.readUint32();
        loc_vars.push_back(LocVar(
            str,
            start,
            end
        ));
    }
    return loc_vars;
}

vector<string> binaryChunk::readUpvalueNames(){
    int size = reader.readUint32();
    vector<string> upvalue_names;
    upvalue_names.reserve(size);
    for(int i = 0; i < size; i++){
        upvalue_names.push_back(reader.readString());
    }
    return upvalue_names;
}

Prototype * binaryChunk::readProto(const string& parent_source){
    Prototype * proto = new Prototype;
    if(mainFunc == nullptr)mainFunc = proto;
    string source = reader.readString();
    if(source == ""){
        source = parent_source;
    }
    proto->Source = source;
    proto->LineDefined = reader.readUint32();
    proto->LastLineDefined = reader.readUint32();
    proto->NumParams = reader.readByte();
    proto->IsVararg = reader.readByte();
    proto->MaxStackSize = reader.readByte();
    proto->Code = readCode();
    proto->Constants = readConstants();
    proto->Upvalues = readUpvalues();
    proto->Protos = readProtos(source);
    proto->LineInfo = readLineInfo();
    proto->LocVars = readLocVars();
    proto->UpvalueNames = readUpvalueNames();
    return proto;
}

void binaryChunk::printHeader(Prototype * f){
    string varargFlag;
    varargFlag= (f->IsVararg) > 0 ? "+" : "";
    string funcType;
    funcType = f->LineDefined > 0 ? "function" : "main";
    printf("\n%s <%s:%d,%d> (%d instructions)\n", funcType.c_str(), f->Source.c_str(), f->LineDefined, f->LastLineDefined, f->Code.size());
    printf("%d%s params, %d slots, %d upvalues, ", 
            f->NumParams, varargFlag.c_str(), f->MaxStackSize, f->Upvalues.size());
    printf("%d locals, %d constants, %d functions\n",
            f->LocVars.size(), f->Constants.size(), f->Protos.size());
}

void binaryChunk::printOperands(Instruction & ins){
    switch (ins.GetOpMode()){
        case IABC:{
            vector<int> abc = ins.ABC();
            printf("%d", abc[0]);
            if(ins.GetBMode() != OpArgN){
                if(abc[1] > 0xff){
                    printf(" %d", -1 - (abc[1] & 0xFF));
                } else{
                    printf(" %d", abc[1]);
                }
            }
            if(ins.GetCMode() != OpArgN){
                if(abc[2] > 0xff){
                    printf(" %d", -1 - (abc[1] & 0xFF));
                } else{
                    printf(" %d", abc[2]);
                }
            }
            break;
        }
        case IABx: {
            vector<int> abx = ins.ABx();
            printf("%d", abx[0]);
            if(ins.GetBMode() == OpArgU){
                printf(" %d", abx[1]);
            }
            else if(ins.GetBMode() == OpArgK){
                printf(" %d", -1 - abx[1]);
            }
            break;
        }
        case IAsBx:{
            vector<int> asbx = ins.AsBx();
            printf("%d, %d", asbx[0], asbx[1]);
            break;
        }
        case IAx: {
            int ax = ins.Ax();
            printf("%d", -1 - ax);
        }
    };
}

void binaryChunk::printCode(Prototype * f){
    cout << "LineInfo Num " << f->LineInfo.size() << endl;
    for(int i = 0; i < f->Code.size(); i++){
        string line = f->LineInfo[i] > 0 ? to_string( (int) (f->LineInfo[i])) : "-";
        Instruction ins = Instruction(f->Code[i]);
        printf("\t%d\t[%s]\t%hx\t%s \t", i + 1, line.c_str(), ins.GetOpCode(), ins.GetOpName().c_str());
        printOperands(ins);
        printf("\n");
    }
}

string constantToString(const TValue & v){
    switch (v.type){
        case TAG_NIL: return "nil";
        case TAG_BOOLEAN: return v.value.b ? "True" : "False";
        case TAG_LONG_STR: return *(v.value.str);
        case TAG_SHORT_STR: return *(v.value.str);
        case TAG_INTEGER: return to_string(v.value.i);
        case TAG_NUMBER: return to_string(v.value.n);
        default: return "?";
    }
}

void binaryChunk::printDetail(Prototype * f){
    printf("Constants (%d) : \n", f->Constants.size());
    for(int i = 0; i < f->Constants.size(); i++){
        printf("\t%d\t%s\n", i + 1, constantToString(f->Constants[i]).c_str());
    }

    printf("locals (%d) :\n", f->LocVars.size());
    for(int i = 0; i < f->LocVars.size(); i++){
        printf("\t%d\t%s\t%d\t%d\n",
        i, f->LocVars[i].VarName.c_str(), f->LocVars[i].StartPC + 1, f->LocVars[i].EndPC + 1);
    }

    printf("upvalues (%d) :\n", f->Upvalues.size());
    for(int i = 0; i < f->Upvalues.size(); i++){
        printf("\t%d\t%s\t%d\t%d\n",
        i, (f->UpvalueNames.size() > 0 ? f->UpvalueNames[i] : "-").c_str(), f->Upvalues[i].Instack, f->Upvalues[i].Idx);
    }
}


void binaryChunk::List(Prototype* f){
    printHeader(f);
    printCode(f);
    printDetail(f);
    for(auto i : f->Protos){
        List(i);
    }
}
void binaryChunk::printList(){
    List(mainFunc);
}