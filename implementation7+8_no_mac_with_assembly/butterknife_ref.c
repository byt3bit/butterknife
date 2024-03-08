
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef __cplusplus
#include <stdalign.h>   // C11 defines _Alignas().  This header defines alignas()
#endif

#include "helpers.h"
#include "butterknife.h"


void BUTTERKNIFE_TWO_BRANCHES(__m128i *U, __m128i *V, __m128i X, __m128i TK1, __m128i *RTKs){

  // RTKs is precomputed TK2 xor RC
  __m128i s, Y0, TK1b;
  __m128i ZERO	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 );   	
  const __m128i H_PERMUTATION = constant8( 1,6,11,12,5,10,15,0,9,14,3,4,13,2,7,8 );

  // Before branching
  s = xor(X, xor(RTKs[0], TK1)); TK1 = permute(TK1, H_PERMUTATION); // XOR
  s = enc(s, xor(RTKs[1], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[2], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[3], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[4], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[5], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[6], TK1)); TK1 = permute(TK1, H_PERMUTATION);// AES ROUND
  s = enc(s, ZERO); // AES ROUND

  // Output is 2n-bit (two branches computed)
  Y0 = s;
  TK1b = TK1;
  
  // First branch
  s = xor(s, xor(RTKs[7], TK1)); TK1 = permute(TK1, H_PERMUTATION); // XOR 
  s = enc(s, xor(RTKs[8], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[9], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[10], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[11], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[12], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[13], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[14], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[15], TK1));
  *U = xor( Y0, s); // AES ROUND

  // Second branch
  s = Y0;
  TK1 = TK1b;

  s = xor(s, xor(RTKs[16], TK1)); TK1 = permute(TK1, H_PERMUTATION); // XOR
  s = enc(s, xor(RTKs[17], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[18], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[19], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[20], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[21], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[22], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[23], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[24], TK1)); // AES ROUND
  
  *V = xor( Y0, s);
}

void BUTTERKNIFE_FULL(uint8_t TPRI_output[NB_BRANCHES*BLOCKSIZE], __m128i X, __m128i TK1, __m128i *RTKs){

  // RTKs is precomputed TK2 xor RC

  __m128i s, Y0, TK1b;
  __m128i ZERO	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 );   	
  const __m128i H_PERMUTATION = constant8( 1,6,11,12,5,10,15,0,9,14,3,4,13,2,7,8 );
  int j;

  // Before branching
  s = xor(X, xor(RTKs[0], TK1)); TK1 = permute(TK1, H_PERMUTATION); // XOR
  s = enc(s, xor(RTKs[1], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[2], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[3], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[4], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[5], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
  s = enc(s, xor(RTKs[6], TK1)); TK1 = permute(TK1, H_PERMUTATION);// AES ROUND
  s = enc(s, ZERO); // AES ROUND

  Y0 = s;
  //printf("Y0: "); p128_hex_u8(s); 
  TK1b = TK1;
  
  // Branches

  for (j=0;j<NB_BRANCHES;j++){
    s = Y0;
    TK1 = TK1b;
  
    s = xor(s, xor(RTKs[j*(R1+1)+7], TK1)); TK1 = permute(TK1, H_PERMUTATION); // XOR
    s = enc(s, xor(RTKs[j*(R1+1)+8], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+9], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+10], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+11], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+12], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+13], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+14], TK1)); TK1 = permute(TK1, H_PERMUTATION); // AES ROUND
    s = enc(s, xor(RTKs[j*(R1+1)+15], TK1)); // AES ROUND

    store(&TPRI_output[j*BLOCKSIZE], xor(s, Y0));

  }
}
