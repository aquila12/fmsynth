// fmrender.h
// FM Output Renderer

#include "fmrender.h"
#include "unistd.h"
#include "stdlib.h"

/* This module owns the root patch array */
int n_patches = 0;
fmpatch_t *patch = 0;

void fmrender_alloc(int patches) {
  n_patches = patches;
  patch = calloc(n_patches, sizeof(fmpatch_t));
}

/* NB: All patches should have been initialized */
void fmrender_free() {
  for(int i=0; i<n_patches; ++i)
    fmpatch_free(&patch[i]);
  free(patch);
  patch = 0;
}

void fmrender_samples(int samples) {
  uint32_t sample;
  int16_t outbuf[BUFSIZE];
  int j=0;

  for(int i=0; i<samples; ++i) {
    sample = 0;
    for(int p=0; p<n_patches; ++p)
      sample += fmpatch_sample(&patch[p]);

    outbuf[j++] = sample * GAIN >> 16;
    if(j==BUFSIZE) {
      write(1, outbuf, j * sizeof(int16_t)); // Ignoring failed writes
      j=0;
    }
  }
  write(1, outbuf, j * sizeof(int16_t));
}
