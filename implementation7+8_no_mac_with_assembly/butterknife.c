
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef __cplusplus
#include <stdalign.h>   // C11 defines _Alignas().  This header defines alignas()
#endif

//////////
// MACROS
//////////
#include "helpers.h"
#include "butterknife.h"

void IEnc_x8(uint8_t* ct, __m128i U, __m128i* RTKs, uint8_t* pt, size_t pt_len){

  int i,j,k;
  int nbButterKnives = pt_len/(BLOCKSIZE*NB_BRANCHES);
  int nbEightButterKnives = nbButterKnives/NB_AES;
  int nbSingleButterKnives = nbButterKnives%8;
  int nbBytesFinalBlock = pt_len % (BLOCKSIZE*NB_BRANCHES);
  int nbBranchesFinalBlock = ((nbBytesFinalBlock % BLOCKSIZE) > 0)? (nbBytesFinalBlock/BLOCKSIZE+1) : (nbBytesFinalBlock/BLOCKSIZE);
  int nbBytesFinalBranch = nbBytesFinalBlock % (BLOCKSIZE);
  bool partialFinalBranch = nbBytesFinalBranch > 0;

  __m128i States[8], Y0s[8];
  __m128i s, Y0;

  uint8_t ptFinalBlock[BLOCKSIZE], ctFinalBlock[BLOCKSIZE];
  __m128i ptBlock;
  __m128i cntU = U; 
  __m128i ZERO	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 );   	
  // Constants to remove data-dependencies in counter-chain
  __m128i ADD_ONE	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1 );   	
  __m128i ADD_TWO	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,2 );   	
  __m128i ADD_THREE	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,3 );   	
  __m128i ADD_FOUR	= constant8( 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,4 );   	

  /////////////////////////////////////////
  // EIGHT BUTTERKNIVES FOR BULK OF MESSAGE
  /////////////////////////////////////////
  for(i=0; i < nbEightButterKnives; i++){

    LOAD_8_STATES(States, cntU);

    // BEFORE BRANCHING
    DEOXYS_BEFORE(States, RTKs);
    SAVE(Y0s, States);

    // OTHER BRANCHES
    for (j = 0; j < NB_BRANCHES; j++){ // compiler unrolls better than I do
      SAVE(States, Y0s);
      DEOXYS_AFTER(States, (&RTKs[R0+(R1+1)*j]));
      EIGHT_XORS(States,Y0s);
      STORE_8_CIPHERTEXTS(States, ct+CT_LEN*NB_AES*i+BLOCKSIZE*j, pt+CT_LEN*NB_AES*i+BLOCKSIZE*j);
    }

  }

  ////////////////////////////////////
  // SINGLE BUTTERKNIVES FOR REMAINDER
  ////////////////////////////////////
  for(i=0; i < nbSingleButterKnives; i++){

    // BEFORE BRANCHING
    BEFORE_SINGLE(s, cntU, RTKs);
    Y0 = s;

    // OTHER BRANCHES
    for (j=0;j<NB_BRANCHES;j++){
      s = Y0;
      AFTER_SINGLE(s, (&RTKs[R0+(R1+1)*j]));
      s = xor(s,Y0);
      store(ct+(nbEightButterKnives*NB_AES+i)*CT_LEN+j*BLOCKSIZE, xor(s, load(pt+(nbEightButterKnives*NB_AES+i)*CT_LEN+j*BLOCKSIZE)));
    }

    cntU = add64(cntU, ADD_ONE);
    }



    //////////////////////////////////////////////
    // FINAL INCOMPLETE BUTTERKNIFE (if necessary)
    //////////////////////////////////////////////
    if(nbBranchesFinalBlock > 0){
      // Before branching
      BEFORE_SINGLE(s, cntU, RTKs);
      Y0 = s;

      // First branch
      AFTER_SINGLE(s, (&RTKs[R0]));
      s = xor(s,Y0);

      // Make sure not to go load/store outside of buffer bounds
      if (partialFinalBranch){
        for (k=0; k < nbBytesFinalBranch; k++){ptFinalBlock[k] = pt[(pt_len/BLOCKSIZE)*BLOCKSIZE+k];}
        for (k=nbBytesFinalBranch; k < BLOCKSIZE; k++){ptFinalBlock[k] = 0;}
      }

        // Make sure not to go load/store outside of buffer bounds
      if (partialFinalBranch & (nbBranchesFinalBlock == 1)){
        ptBlock = load(ptFinalBlock);
        store(ctFinalBlock, xor(s, ptBlock));
        for (k=0; k < nbBytesFinalBranch; k++){ct[(pt_len/BLOCKSIZE)*BLOCKSIZE+k] = ctFinalBlock[k];}
      } else {
        ptBlock = load(pt+(nbEightButterKnives*NB_AES+(nbSingleButterKnives))*CT_LEN);
        store(ct+(nbEightButterKnives*NB_AES+(nbSingleButterKnives))*CT_LEN, xor(s, ptBlock));
      }

      // Other branches
      for (j=1;j<nbBranchesFinalBlock;j++){
        s = Y0;
        AFTER_SINGLE(s, (&RTKs[R0+(R1+1)*j]));
        s = xor(s,Y0);

        // Make sure not to go load/store outside of buffer bounds
        if (partialFinalBranch & (j == nbBranchesFinalBlock-1)){
          ptBlock = load(ptFinalBlock);
          store(ctFinalBlock, xor(s, ptBlock));
          for (k=0; k < nbBytesFinalBranch; k++){ct[(pt_len/BLOCKSIZE)*BLOCKSIZE+k] = ctFinalBlock[k];}
        } else {
          ptBlock = load(pt+(nbEightButterKnives*NB_AES+(nbSingleButterKnives))*CT_LEN+j*BLOCKSIZE);
          store(ct+(nbEightButterKnives*NB_AES+(nbSingleButterKnives))*CT_LEN+j*BLOCKSIZE, xor(s, ptBlock));
        }

      }

  }
}

