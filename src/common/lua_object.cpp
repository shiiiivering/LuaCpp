#include "common/lua_object.h"
#include "common/lua.h"
#include "number/lua_math.h"
#include "state/lua_table.h"
#include "state/lua_closure.h"
#include <string>
#include <vector>

// compare functions
bool string_eq(const TValue & a, const TValue & b){
    return *(a.value.str) == *(b.value.str);
}
bool float_eq(const TValue & a, const TValue & b){
    return a.value.n == b.value.n;
}
bool integer_eq(const TValue & a, const TValue & b){
    return a.value.i == b.value.i;
}
bool bool_eq(const TValue & a, const TValue & b){
    return a.value.b == b.value.b;
}
bool nil_eq(const TValue & a, const TValue & b){
    return a.type == LUA_TNIL && b.type == LUA_TNIL;
}

typedef bool (*tvalue_eq)(const TValue & a, const TValue & b) ;
vector<tvalue_eq> eqFunctions;

// hash functions
size_t string_hash(const TValue & a){
    return hash<string>()(*(a.value.str));
}
size_t float_hash(const TValue & a){
    return hash<lua_Number> () (a.value.n);
}
size_t integer_hash(const TValue & a){
    return hash<lua_Integer>()(a.value.i);
}
size_t bool_hash(const TValue & a){
    return hash<bool>()(a.value.b);
}
size_t nil_hash(const TValue & a){
    return 0;
}


typedef size_t (*tvalue_hash)(const TValue & a);
vector<tvalue_hash> hashFunctions;
void init_function_table(){
    if(eqFunctions.empty()){
        eqFunctions = vector<tvalue_eq>(LUA_MAX_TYPE_INDEX, nullptr);
        eqFunctions[LUA_TBOOLEAN] = bool_eq;
        eqFunctions[LUA_NUMINT] = integer_eq;
        eqFunctions[LUA_NUMFLT] = float_eq;
        eqFunctions[LUA_SHRSTR] = string_eq;
        eqFunctions[LUA_LNGSTR] = string_eq;
        eqFunctions[LUA_TNIL] = nil_eq;
    }
    if(hashFunctions.empty()){
        hashFunctions = vector<tvalue_hash>(LUA_MAX_TYPE_INDEX, nullptr);
        hashFunctions[LUA_TBOOLEAN] = bool_hash;
        hashFunctions[LUA_NUMINT] = integer_hash;
        hashFunctions[LUA_NUMFLT] = float_hash;
        hashFunctions[LUA_SHRSTR] = string_hash;
        hashFunctions[LUA_LNGSTR] = string_hash;
        hashFunctions[LUA_TNIL] = nil_hash;
    }
}


TValue::~TValue(){
    if(type == LUA_LNGSTR || type == LUA_SHRSTR) delete (value.str);
    else if(type == LUA_TTABLE && value.t){
        value.t->ref--;
        if(value.t->ref == 0){
            delete value.t;
        }
    }
    else if(type == LUA_TFUNCTION && value.c){
        value.c->ref--;
        if(value.c->ref == 0){
            delete value.c;
        }
    }
}

TValue::TValue(luaTable * t):type(LUA_TTABLE){
    value.t = t; 
    t->ref++;
}
TValue::TValue(luaClosure * c):type(LUA_TFUNCTION){
    value.c = c;
    c->ref++;
}

TValue::TValue(const TValue & old){
    value.i = old.value.i;
    type = old.type;
    if(type == LUA_LNGSTR || type == LUA_SHRSTR){
        value.str = new string(*(old.value.str));
    }
    else if(type == LUA_TTABLE){
        value.t->ref++;
    }
    else if(type == LUA_TFUNCTION){
        value.c->ref++;
    }
}
TValue::TValue(TValue && old){
    value.i = old.value.i;
    type = old.type;
    if(type == LUA_LNGSTR || type == LUA_SHRSTR){
        old.value.str = nullptr;
    }
    if(type == LUA_TTABLE){
        old.value.t = nullptr;
    }
    if(type == LUA_TFUNCTION){
        old.value.c = nullptr;
    }
}

TValue & TValue::operator = (TValue && old){
    if(type == LUA_LNGSTR || type == LUA_SHRSTR){
        delete value.str;
    }
    if(type == LUA_TTABLE && value.t){
        value.t->ref--;
        if(value.t->ref == 0){
            delete value.t;
        }
    }
    if(type == LUA_TFUNCTION && value.c){
        value.c->ref--;
        if(value.c->ref == 0){
            delete value.c;
        }
    }
    type = old.type;
    if(type == LUA_LNGSTR || type == LUA_SHRSTR){
        value.str = old.value.str;
        old.value.str = nullptr;
    }
    else if(type == LUA_TTABLE){
        value.t = old.value.t;
        old.value.t = nullptr;
    }
    else if(type == LUA_TFUNCTION){
        value.c = old.value.c;
        old.value.c = nullptr;
    }
    else{
        value.i = old.value.i;
    }
    return * this;
}
TValue & TValue::operator = (const TValue & old){
    TValue temp(old);
    *this = std::move(temp);
    return * this;
}

bool TValue::operator == (const TValue & p) const{
    if(p.type != type)return false;
    init_function_table();
    return eqFunctions[p.type](* this, p);
}

size_t TValueHash::operator () (const TValue & a) const noexcept{
    init_function_table();
    return hashFunctions[a.type](a);
}

lua_Number TValue::convertToFloat(int * status) const {
    switch(type){
        case LUA_NUMFLT: if(status)*status = LUA_OK; return value.n;
        case LUA_NUMINT: if(status)*status = LUA_OK; return value.i;
        case LUA_SHRSTR:
        case LUA_LNGSTR:
                         return ParseFloat(*(value.str), status);
        default: if(status)*status = LUA_ERROR; return 0;
    }
}
lua_Integer TValue::convertToInteger(int * status) const {
    switch(type){
        case LUA_NUMINT: if(status)*status = LUA_OK; return value.i;
        case LUA_NUMFLT: return FloatToInteger(value.n, status);
        case LUA_SHRSTR:
        case LUA_LNGSTR:
                         return ParseInteger(*(value.str), status);
        default: if(status)*status = LUA_ERROR; return 0;
    }
}

lua_Integer TValue::_stringToInteger(int * status) const {
    int ok = LUA_OK;
    lua_Integer i = ParseInteger(*(value.str), &ok);
    if(ok != LUA_OK){
        if(status)*status = LUA_ERROR;
        return 0;
    }
    lua_Number f = ParseFloat(*(value.str), &ok);
    if(FLOAT_INTEGER_EQUAL(f, i)){
        if(status)*status = LUA_OK;
        return i;
    }
    else{
        if(status)*status = LUA_ERROR;
        return i;
    }
}

void TValue::print() const {
        switch (type){
            case LUA_TBOOLEAN: printf("[%s]", value.b ? "true" : "false");
            break;
            case LUA_NUMFLT: printf("[%f]", value.n);
            break;
            case LUA_NUMINT: printf("[%d]", value.i);
            break;
            case LUA_SHRSTR: 
            case LUA_LNGSTR:
                printf("[\"%s\"]", value.str->c_str());
                break;
            case LUA_TFUNCTION:
                cout << "[function: " ;
                for(auto p : value.c->upvals){
                     p->get()->print();
                }
                cout << "]";
                break;
            default: printf("[type %d]", type);
        }
    }

void TValue::printTable() const{
    if(type == LUA_TTABLE){
        auto table = value.t;
        if(table == nullptr){
            cout << "table is NULL" << endl;
        }
        cout << "arr: " << endl;
        for(auto i : table->arr){
            i.print();
        }
        cout << endl;
        cout << "map: " << endl;
        for(auto i : table->_map){
            cout << "(";
            i.first.print();
            cout << ", ";
            i.second.print();
            cout << ")";
        }
        cout << endl;
        if(table->metatable.type == LUA_TTABLE){
            cout << "metatable: -----------" << endl;
            table->metatable.printTable();
        }
    }
}