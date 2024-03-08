#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "helpers.h"
#include "butterknife.h"

void fenc_ref(uint8_t* C, uint8_t u[16], uint8_t v[16], uint8_t key[16], uint8_t* pt, size_t pt_len){

  int i, j;
  __m128i U = load(u), V = load(v);
  __m128i ADD_ONE	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 );   	
  __m128i RTKs[R0+(R1+1)*NB_BRANCHES], subkeysTK2[R0+R1+1];
  __m128i RCONS[R0+(R1+1)*NB_BRANCHES];

  __m128i mask_top_7_bits        = constant8_same(0xfe);
  __m128i mask_bottom_1_bit      = constant8_same(0x01);
  const __m128i H_PERMUTATION = constant8( 1,6,11,12,5,10,15,0,9,14,3,4,13,2,7,8 );

  uint8_t TPRI_output[NB_BRANCHES*BLOCKSIZE];
  
  int nbButterKnives = pt_len/(BLOCKSIZE*NB_BRANCHES);
  int nbBytesFinalBlock = pt_len % (BLOCKSIZE*NB_BRANCHES);

  ///////////////////
  // PRECOMPUTATIONS
  ///////////////////
  // RC before branching 
  for(i=0; i<R0; i++){ RCONS[i] = constant8( 1,2,4,8, rcon[i], rcon[i], rcon[i], rcon[i],  0,0,0,0,  0,0,0,0); }

  // RC in each of the branches
  for (j=0; j < NB_BRANCHES; j++){
    for(i=0; i<R1+1; i++){ RCONS[R0+(R1+1)*j+i] = constant8( 1,2,4,8, rcon[R0+i], rcon[R0+i], rcon[R0+i], rcon[R0+i],  j+1,j+1,j+1,j+1,  0,0,0,0); }
  }

  // Pre-compute RTKs as TK2 xor RC for the relevant branch
  subkeysTK2[0] = load(key);
  TWEAKEY_SCHEDULE_FIRST_BRANCH(subkeysTK2, RTKs);
  for (j=1; j < NB_BRANCHES; j++){ TWEAKEY_SCHEDULE_OTHER_BRANCHES(subkeysTK2, (&RTKs[R0+(R1+1)*j]), (&RCONS[R0+(R1+1)*j])); }

  //////////////////
  // IEnc (K, I, C)
  //////////////////
  
  for(i=0; i < nbButterKnives; i++) {
    BUTTERKNIFE_FULL(TPRI_output, U, V, RTKs);
    for(j=0; j<NB_BRANCHES*BLOCKSIZE;j++){
      C[i*NB_BRANCHES*BLOCKSIZE+j] = pt[i*NB_BRANCHES*BLOCKSIZE+j] ^ TPRI_output[j];
    }
    U = add64(U, ADD_ONE);
  }

  // Final BUTTERKNIFE
  if (pt_len % (NB_BRANCHES*BLOCKSIZE) > 0){
    BUTTERKNIFE_FULL(TPRI_output, U, V, RTKs);
    for(j=0; j<nbBytesFinalBlock;j++){
      C[nbButterKnives*NB_BRANCHES*BLOCKSIZE+j] = pt[nbButterKnives*NB_BRANCHES*BLOCKSIZE+j] ^ TPRI_output[j];
    }
  }

}
