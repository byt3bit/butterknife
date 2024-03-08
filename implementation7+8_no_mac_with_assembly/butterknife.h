#ifndef BUTTERKNIFE_H
#define BUTTERKNIFE_H

#include <stdint.h>
#include <xmmintrin.h>              /* SSE instructions and _mm_malloc */
#include <emmintrin.h>              /* SSE2 instructions               */
#include <wmmintrin.h>              /* SSSE3 instructions              */
#include <tmmintrin.h>              /* SSSE3 instructions              */



//////////////
// PARAMETERS
//////////////
//#define MSG_SIZE_LONG 65536
#define MSG_SIZE_LONG 65536

// For readability, do not modify these
#define R0 7
#define R1 8
#define NB_AES 8
#define BLOCKSIZE 16
#define NB_BRANCHES 8
#define CT_LEN (BLOCKSIZE*NB_BRANCHES)

//////////////
// Functions
//////////////
void BUTTERKNIFE_TWO_BRANCHES(__m128i *U, __m128i *V, __m128i X, __m128i TK1, __m128i *RTKs); // Ref
void BUTTERKNIFE_FULL(uint8_t TPRI_output[NB_BRANCHES*BLOCKSIZE], __m128i X, __m128i TK1, __m128i *RTKs); // Ref
void IEnc_x8(uint8_t* ct, __m128i U, __m128i* RTKs, uint8_t* pt, size_t pt_len); // Opt

//////////////
// INTRINSICS
//////////////
#define enc(a,b) _mm_aesenc_si128(a,b) 
#define xor(a,b) _mm_xor_si128(a,b) 
#define and(a,b) _mm_and_si128(a,b) 
#define or(a,b) _mm_or_si128(a,b) 
#define load(p)       _mm_load_si128((__m128i *)(p))
#define add32(a,b) _mm_add_epi32 (a,b)
#define add64(a,b) _mm_add_epi64 (a,b)
#define store(p,x)    _mm_store_si128((__m128i *)(p), (x))
#define constant8(b15,b14,b13,b12,b11,b10,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0)   _mm_set_epi8 ((b0),(b1),(b2),(b3),(b4),(b5),(b6),(b7),(b8),(b9),(b10),(b11),(b12),(b13),(b14),(b15))
#define constant32(b3,b2,b1,b0)   	_mm_set_epi32((b0),(b1),(b2),(b3))
#define constant8_same(a) 			_mm_set1_epi8(a)
#define permute(a,b) _mm_shuffle_epi8(a,b)  // Shuffle packed 8-bit integers in a according to shuffle control mask in b

// Rounds constants
static const uint8_t rcon[17] = {
    0x2f,0x5e,0xbc,0x63,0xc6,0x97,0x35,0x6a,
    0xd4,0xb3,0x7d,0xfa,0xef,0xc5,0x91,0x39,
    0x72
};

////////////////////
// TWEAKEY SCHEDULE
////////////////////

// DEOXYS LFSR
#define LFSR( key, new_key ) \
  new_key = xor ( _mm_and_si128( xor(_mm_srli_epi32( key, 7 ),_mm_srli_epi32( key, 5 ) ), mask_bottom_1_bit), _mm_and_si128( mask_top_7_bits  , _mm_slli_epi32( key, 1 )));\

// Tweakey schedule round for TK2
  // This is used after branching (data dependency on TK1)
#define ONE_KEY_ROUND_TK2(key, new_key)\
    LFSR( key, new_key );\
    new_key = permute( new_key, H_PERMUTATION);


/* For the rounds before branching as well as the first branch, 
 *  produce the part of RTKs that can be precomputed (i.e., TK2 xor RC)
      - TK1 will be added on the fly in the two BUTTERKNIFE calls in GiMAC
      - For the IEnc pass, precomputed TK1 will be added to this 
          (by invoking TWEAKEY_SCHEDULE_PRECOMPUTE_TK1)
 */
#define TWEAKEY_SCHEDULE_FIRST_BRANCH( subkeysTK2, RTKs)\
  RTKs[ 0] = xor( subkeysTK2[0], RCONS[ 0] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[0], subkeysTK2[1]);\
  RTKs[ 1] = xor( subkeysTK2[1], RCONS[ 1] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[1], subkeysTK2[2]);\
  RTKs[ 2] = xor( subkeysTK2[2], RCONS[ 2] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[2], subkeysTK2[3]);\
  RTKs[ 3] = xor( subkeysTK2[3], RCONS[ 3] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[3], subkeysTK2[4]);\
  RTKs[ 4] = xor( subkeysTK2[4], RCONS[ 4] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[4], subkeysTK2[5]);\
  RTKs[ 5] = xor( subkeysTK2[5], RCONS[ 5] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[5], subkeysTK2[6]);\
  RTKs[ 6] = xor( subkeysTK2[6], RCONS[ 6] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[6], subkeysTK2[7]);\
  RTKs[ 7] = xor( subkeysTK2[7], RCONS[ 7] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[7], subkeysTK2[8]);\
  RTKs[ 8] = xor( subkeysTK2[8], RCONS[ 8] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[8], subkeysTK2[9]);\
  RTKs[ 9] = xor( subkeysTK2[9], RCONS[ 9] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[9], subkeysTK2[10]);\
  RTKs[ 10] = xor( subkeysTK2[10], RCONS[ 10] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[10], subkeysTK2[11]);\
  RTKs[ 11] = xor( subkeysTK2[11], RCONS[ 11] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[11], subkeysTK2[12]);\
  RTKs[ 12] = xor( subkeysTK2[12], RCONS[ 12] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[12], subkeysTK2[13]);\
  RTKs[ 13] = xor( subkeysTK2[13], RCONS[ 13] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[13], subkeysTK2[14]);\
  RTKs[ 14] = xor( subkeysTK2[14], RCONS[ 14] ); \
  ONE_KEY_ROUND_TK2(subkeysTK2[14], subkeysTK2[15]);\
  RTKs[ 15] = xor( subkeysTK2[15], RCONS[ 15] ); \


/* For the rounds before branching as well as the first branch, 
 *  produce the part of RTKs that can be precomputed (i.e., TK2 xor RC)
      - TK1 has to be precomputed for two distinct values
 */
#define TWEAKEY_SCHEDULE_OTHER_BRANCHES(subkeysTK2, RTKs, rc)\
  RTKs[ 0] = xor (subkeysTK2[7], rc[0]);\
  RTKs[ 1] = xor (subkeysTK2[8], rc[1]);\
  RTKs[ 2] = xor (subkeysTK2[9], rc[2]);\
  RTKs[ 3] = xor (subkeysTK2[10], rc[3]);\
  RTKs[ 4] = xor (subkeysTK2[11], rc[4]);\
  RTKs[ 5] = xor (subkeysTK2[12], rc[5]);\
  RTKs[ 6] = xor (subkeysTK2[13], rc[6]);\
  RTKs[ 7] = xor (subkeysTK2[14], rc[7]);\
  RTKs[ 8] = xor (subkeysTK2[15], rc[8]);\

/*
 * During the IEnc pass, TK1 is the same for all BUTTERKNIFEs (i.e., V)
 *  As a result, we can precompute the values of all round tweakeys. After this function:
 */
#define TWEAKEY_SCHEDULE_PRECOMPUTE_TK1(subkeysTK1, RTKs)\
  RTKs[ 0] = xor(RTKs[0], subkeysTK1[0]); subkeysTK1[1] = permute(subkeysTK1[0], H_PERMUTATION);\
  RTKs[ 1] = xor(RTKs[1], subkeysTK1[1]); subkeysTK1[2] = permute(subkeysTK1[1], H_PERMUTATION);\
  RTKs[ 2] = xor(RTKs[2], subkeysTK1[2]); subkeysTK1[3] = permute(subkeysTK1[2], H_PERMUTATION);\
  RTKs[ 3] = xor(RTKs[3], subkeysTK1[3]); subkeysTK1[4] = permute(subkeysTK1[3], H_PERMUTATION);\
  RTKs[ 4] = xor(RTKs[4], subkeysTK1[4]); subkeysTK1[5] = permute(subkeysTK1[4], H_PERMUTATION);\
  RTKs[ 5] = xor(RTKs[5], subkeysTK1[5]); subkeysTK1[6] = permute(subkeysTK1[5], H_PERMUTATION);\
  RTKs[ 6] = xor(RTKs[6], subkeysTK1[6]); subkeysTK1[7] = permute(subkeysTK1[6], H_PERMUTATION);\
  RTKs[ 7] = xor(RTKs[7], subkeysTK1[7]); subkeysTK1[8] = permute(subkeysTK1[7], H_PERMUTATION);\
  RTKs[ 8] = xor(RTKs[8], subkeysTK1[8]); subkeysTK1[9] = permute(subkeysTK1[8], H_PERMUTATION);\
  RTKs[ 9] = xor(RTKs[9], subkeysTK1[9]); subkeysTK1[10] = permute(subkeysTK1[9], H_PERMUTATION);\
  RTKs[ 10] = xor(RTKs[10], subkeysTK1[10]); subkeysTK1[11] = permute(subkeysTK1[10], H_PERMUTATION);\
  RTKs[ 11] = xor(RTKs[11], subkeysTK1[11]); subkeysTK1[12] = permute(subkeysTK1[11], H_PERMUTATION);\
  RTKs[ 12] = xor(RTKs[12], subkeysTK1[12]); subkeysTK1[13] = permute(subkeysTK1[12], H_PERMUTATION);\
  RTKs[ 13] = xor(RTKs[13], subkeysTK1[13]); subkeysTK1[14] = permute(subkeysTK1[13], H_PERMUTATION);\
  RTKs[ 14] = xor(RTKs[14], subkeysTK1[14]); subkeysTK1[15] = permute(subkeysTK1[14], H_PERMUTATION);\
  RTKs[ 15] = xor(RTKs[15], subkeysTK1[15]); \
  
#define TWEAKEY_SCHEDULE_PRECOMPUTE_TK1_OTHER_BRANCHES(subkeysTK1, RTKs)\
  RTKs[ 0] = xor(RTKs[0], subkeysTK1[7]);\
  RTKs[ 1] = xor(RTKs[1], subkeysTK1[8]);\
  RTKs[ 2] = xor(RTKs[2], subkeysTK1[9]);\
  RTKs[ 3] = xor(RTKs[3], subkeysTK1[10]);\
  RTKs[ 4] = xor(RTKs[4], subkeysTK1[11]);\
  RTKs[ 5] = xor(RTKs[5], subkeysTK1[12]);\
  RTKs[ 6] = xor(RTKs[6], subkeysTK1[13]);\
  RTKs[ 7] = xor(RTKs[7], subkeysTK1[14]);\
  RTKs[ 8] = xor(RTKs[8], subkeysTK1[15]);\


  // Macro to prepare plaintext input for 8 BUTTERKNIFE instances
  #define LOAD_8_STATES( States , cntU ) \
    States[0] = cntU;\
    States[1] = add64(cntU, ADD_ONE);\
    States[2] = add64(cntU, ADD_TWO);\
    States[3] = add64(cntU, ADD_THREE);\
    cntU = add64(cntU, ADD_FOUR);\
    States[4] = cntU;\
    States[5] = add64(cntU, ADD_ONE);\
    States[6] = add64(cntU, ADD_TWO);\
    States[7] = add64(cntU, ADD_THREE);\
    cntU = add64(cntU, ADD_FOUR);\


// Macro to xor plaintext with BUTTERKNIFE-generated keystream and store output
#define STORE_8_CIPHERTEXTS( States, ct, pt ) \
	store( ct +   0 , xor(States[0], load(pt + 0)));\
	store( ct + CT_LEN, xor(States[1], load(pt + CT_LEN)));\
	store( ct + 2*CT_LEN, xor(States[2], load(pt + 2*CT_LEN)));\
	store( ct + 3*CT_LEN, xor(States[3], load(pt + 3*CT_LEN)));\
	store( ct + 4*CT_LEN, xor(States[4], load(pt + 4*CT_LEN)));\
	store( ct + 5*CT_LEN, xor(States[5], load(pt + 5*CT_LEN)));\
	store( ct + 6*CT_LEN, xor(States[6], load(pt + 6*CT_LEN)));\
	store( ct + 7*CT_LEN, xor(States[7], load(pt + 7*CT_LEN)));\


// Macro to save the Y0 states
#define SAVE( Y0s, States) \
  Y0s[0] = States[0];\
  Y0s[1] = States[1];\
  Y0s[2] = States[2];\
  Y0s[3] = States[3];\
  Y0s[4] = States[4];\
  Y0s[5] = States[5];\
  Y0s[6] = States[6];\
  Y0s[7] = States[7];\
  Y0s[8] = States[8];\


// Macro to xor the same subkey (i.e., RTK_0 to eight BUTTERKNIFEs)
#define ONE_XOR(s, subkey)\
s[0] = xor( s[0] , subkey );\
s[1] = xor( s[1] , subkey );\
s[2] = xor( s[2] , subkey );\
s[3] = xor( s[3] , subkey );\
s[4] = xor( s[4] , subkey );\
s[5] = xor( s[5] , subkey );\
s[6] = xor( s[6] , subkey );\
s[7] = xor( s[7] , subkey );\

// Macro to xor two tuples of 8 values (i.e., 8 Y_0s to eight BUTTERKNIFEs output blocks)
#define EIGHT_XORS(s, Y0)\
s[0] = xor( s[0] , Y0[0] );\
s[1] = xor( s[1] , Y0[1] );\
s[2] = xor( s[2] , Y0[2] );\
s[3] = xor( s[3] , Y0[3] );\
s[4] = xor( s[4] , Y0[4] );\
s[5] = xor( s[5] , Y0[5] );\
s[6] = xor( s[6] , Y0[6] );\
s[7] = xor( s[7] , Y0[7] );\

// Advance one AES round in eight BUTTERKNIFEs
  // This one is used *before* branching, as the subkey is already fully precomputed
#define ONE_ROUND(s, RTK)\
s[0] = enc( s[0] , RTK );\
s[1] = enc( s[1] , RTK );\
s[2] = enc( s[2] , RTK );\
s[3] = enc( s[3] , RTK );\
s[4] = enc( s[4] , RTK );\
s[5] = enc( s[5] , RTK );\
s[6] = enc( s[6] , RTK );\
s[7] = enc( s[7] , RTK );\

// Compute Y_0 in each of the eight BUTTERKNIFEs before branching
#define DEOXYS_BEFORE( s , RTKs)\
ONE_XOR(s, RTKs[0]);\
ONE_ROUND(s, RTKs[1]);\
ONE_ROUND(s, RTKs[2]);\
ONE_ROUND(s, RTKs[3]);\
ONE_ROUND(s, RTKs[4]);\
ONE_ROUND(s, RTKs[5]);\
ONE_ROUND(s, RTKs[6]);\
ONE_ROUND(s, ZERO);\

// Compute Y_0 in each of the eight BUTTERKNIFEs after branching
#define DEOXYS_AFTER( s , RTKs)\
ONE_XOR(  s, RTKs[0]);  \
ONE_ROUND(s, RTKs[1]);  \
ONE_ROUND(s, RTKs[2]);  \
ONE_ROUND(s, RTKs[3]);  \
ONE_ROUND(s, RTKs[4]);  \
ONE_ROUND(s, RTKs[5]);  \
ONE_ROUND(s, RTKs[6]);  \
ONE_ROUND(s, RTKs[7]);  \
ONE_ROUND(s, RTKs[8]);  \


// Variant of above but for single tablespoon
#define BEFORE_SINGLE( s , cntU, RTKs)\
s = xor(cntU, RTKs[0]); \
s = enc(s, RTKs[1]); \
s = enc(s, RTKs[2]); \
s = enc(s, RTKs[3]); \
s = enc(s, RTKs[4]); \
s = enc(s, RTKs[5]); \
s = enc(s, RTKs[6]); \
s = enc(s, ZERO); \


#define AFTER_SINGLE( s ,RTKs)\
s = xor(s, RTKs[0]); \
s = enc(s, RTKs[1]); \
s = enc(s, RTKs[2]); \
s = enc(s, RTKs[3]); \
s = enc(s, RTKs[4]); \
s = enc(s, RTKs[5]); \
s = enc(s, RTKs[6]); \
s = enc(s, RTKs[7]); \
s = enc(s, RTKs[8]); \

#endif // BUTTERKNIFE_H
