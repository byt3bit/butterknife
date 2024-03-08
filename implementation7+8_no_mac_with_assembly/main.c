#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "helpers.h"
#include "butterknife.h"
#include "fenc.h"

# include "measurements.h"

void longMessageMeasure(){

  printf("===== Performance for long messages =====\n");

  uint8_t k[16] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", u[16] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", v[16] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f";
  uint8_t pt[MSG_SIZE_LONG], ct_ref[MSG_SIZE_LONG], ct[MSG_SIZE_LONG];

  unsigned long j;
  
  srand(time(0)); 

  for (j = 0; j < MSG_SIZE_LONG; j++){ pt[j] = rand() & 0xFF;}

  fenc_ref(ct_ref, u, v, k, pt, MSG_SIZE_LONG);
  fenc(ct, u, v, k, pt, MSG_SIZE_LONG);
  assert(isEqual(ct, ct_ref, MSG_SIZE_LONG));

  RDTSC_MEASURE({
    fenc(ct, u, v, k, pt, MSG_SIZE_LONG);
  })
  
  printf("FENC (long M): %.3f c/B\n", RDTSC_total_clk/(float)((MSG_SIZE_LONG)));
}

int main(){

  longMessageMeasure();

  return 0;
}
