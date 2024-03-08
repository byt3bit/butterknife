#include "helpers.h"

void printArray(uint8_t* s, int sLen){
    int i;
    for(i = 0; i < sLen; i++) {if(i%16 == 0) {printf("\n\t");} printf("%02x ", s[i]);}
        printf("\n");
}

void p128_hex_u8(__m128i in) {
    alignas(16) uint8_t v[16];
    _mm_store_si128((__m128i*)v, in);
    printf(" %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
           v[0], v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],
           v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15]);
}

bool isEqual(unsigned char *a, unsigned char *b, size_t aLen){

    size_t i;
    for(i = 0; i < aLen; i++){
        if(a[i] != b[i]) {printf("WRONG in byte %lu\n", i); return false;};
    }
    return true;
}