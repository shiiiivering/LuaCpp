#include "fpb.h"
// transform between float point byte and integer
int Int2fb(int x){
    int e = 0;
    if(x < 8){
        return x;
    }
    while(x >= (8 << 4)){
        x = (x + 0xf) >> 4; // x = ciel(x / 16)
        e += 4;
    }
    while(x >= (8 << 1)){
        x = (x + 1) >> 1; // x = ciel(x / 2)
        e++;
    }
    return ((e + 1) << 3) | (x - 8);
}
int Fb2int(int x){
    if(x < 8){
        return x;
    }else{
        return ((x & 7) + 8) << ((unsigned int)((x >> 3) - 1));
    }
}