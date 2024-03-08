#ifndef IAE_H
#define IAE_H

#include <stdint.h>

void fenc(uint8_t* C, uint8_t u[16], uint8_t v[16], uint8_t K[16], uint8_t* pt, size_t pt_len);
void fenc_ref(uint8_t* C, uint8_t u[16], uint8_t v[16], uint8_t K[16], uint8_t* pt, size_t pt_len);

#endif
