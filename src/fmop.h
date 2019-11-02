// fmop.h
// Operator handling

#include "fmfunc.h"
#include "stddef.h"

#define ADHR_RATE_CONSTANT ((float)SAMPLE_1/(float)RATE)

typedef sample_t rate_t;  /* ADHR rates */

typedef enum adhr_e {
  adhr_idle = 0,
  adhr_attack,
  adhr_decay,
  adhr_hold,
  adhr_release
} adhr_t;

typedef struct fmop_s {
  frequency_t f0; /* Current oscillator frequency */
  phase_t ph;     /* Current oscillator phase */
  sample_t ampl;  /* Current ADHR amplitude */
  adhr_t mode;    /* Current ADHR state */
  sample_t value; /* Output value */
} fmop_t;

typedef struct fmop_param_s {
  uint8_t abs_freq;     /* Absolute rather than relative f0? */
  union {
    sample_t freq_mul;  /* Note-frequency multiplier */
    frequency_t f0;     /* Absolute frequency */
  };
  rate_t rate[5];       /* xADHR rate - x is unused idle entry */
  sample_t h_level;     /* Hold (sustain) level */
  uint8_t adhr_enable;  /* Envelope unit enable */
} fmop_param_t;

void fmop_update(fmop_t *operator, fmop_param_t *param, sample_t input);
void fmop_reset(fmop_t *operator);
void fmop_param_osc_abs(fmop_param_t *param, float f0);
void fmop_param_osc_rel(fmop_param_t *param, float freq_mul);
void fmop_param_adhr(fmop_param_t *param, float a_rate, float d_rate, float h_rate, float h_level, float r_rate);
