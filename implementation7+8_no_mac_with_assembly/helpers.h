#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <xmmintrin.h>              /* SSE instructions and _mm_malloc */

#ifndef __cplusplus
#include <stdalign.h>   // C11 defines _Alignas().  This header defines alignas()
#endif

void printArray(uint8_t* s, int sLen);
bool isEqual(unsigned char *a, unsigned char *b, size_t aLen);
void p128_hex_u8(__m128i in);

#endif 