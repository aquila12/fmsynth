// fmadhr.h
// Attack-Decay-Hold-Release envelope unit

#include "fmcore.h"
#include "stddef.h"

#define ADHR_RATE_CONSTANT ((float)SAMPLE_1/(float)RATE)

typedef int32_t rate_t;

typedef enum adhr_e {
  adhr_idle = 0,
  adhr_attack,
  adhr_decay,
  adhr_hold,
  adhr_release
} adhr_t;

typedef struct fmadhr_s {
  fmel_t el;
  sample_t *input;
  sample_t ampl;
  rate_t rate[5];
  sample_t h_level;
  adhr_t mode;
} fmadhr_t;

int fmadhr_init(fmadhr_t *adhr, float a_rate, float d_rate, float h_rate, float h_level, float r_rate, sample_t *input);
