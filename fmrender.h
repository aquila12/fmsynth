// fmrender.h
// FM Output Renderer

#include "fmpatch.h"

#define BUFSIZE (RATE / 10)
#define GAIN 16

/* This module owns the root patch array */
extern int n_patches;
extern fmpatch_t *patch;

void fmrender_alloc(int patches);
void fmrender_free();
void fmrender_samples(int samples);
