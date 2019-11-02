// fmop.c
// Operator handling

#include "fmop.h"

sample_t fmop_update_osc(fmop_t *operator, fmop_param_t *param, sample_t input) {
  if(input) operator->ph += MUL(operator->f0, input + SAMPLE_1);
  else operator->ph += operator->f0;

  return sine(operator->ph);
}

sample_t fmop_update_adhr(fmop_t *operator, fmop_param_t *param, sample_t input) {
  if(!param->adhr_enable) {
    if(operator->mode == adhr_release) operator->mode = adhr_idle;
    return input;
  }
  
  if(operator->mode == adhr_idle) return 0;

  operator->ampl += param->rate[operator->mode];

  if(operator->ampl <= 0) {
    operator->mode = adhr_idle;
    operator->ampl = 0;
    return 0;
  }

  if(operator->mode == adhr_attack && operator->ampl >= SAMPLE_1) {
    operator->ampl = SAMPLE_1;
    operator->mode = adhr_decay;
  }

  if(operator->mode == adhr_decay && operator->ampl <= param->h_level) {
    operator->ampl = param->h_level;
    operator->mode = adhr_hold;
  }

  return MUL(input, operator->ampl);
}

void fmop_update(fmop_t *operator, fmop_param_t *param, sample_t input) {
  sample_t signal = fmop_update_osc(operator, param, input);
  operator->value = fmop_update_adhr(operator, param, signal);
}

void fmop_reset(fmop_t *operator) {
  operator->ph = 0;
  operator->f0 = 0;
  operator->ampl = 0;
  operator->mode = adhr_idle;
  operator->value = 0;
}

void fmop_param_osc_abs(fmop_param_t *param, float f0) {
  param->abs_freq = 1;
  param->f0 = f0 * HERTZ;
}

void fmop_param_osc_rel(fmop_param_t *param, float freq_mul) {
  param->abs_freq = 0;
  param->freq_mul = freq_mul * SAMPLE_1;
}

void fmop_param_adhr(fmop_param_t *param,  float a_rate, float d_rate, float h_rate, float h_level, float r_rate) {
  param->adhr_enable = 1;
  param->rate[adhr_idle] = 0;
  param->rate[adhr_attack]  = a_rate * ADHR_RATE_CONSTANT;
  param->rate[adhr_decay]   = -d_rate * ADHR_RATE_CONSTANT;
  param->rate[adhr_hold]    = -h_rate * ADHR_RATE_CONSTANT;
  param->rate[adhr_release] = -r_rate * ADHR_RATE_CONSTANT;
  param->h_level = h_level * SAMPLE_1;
}
