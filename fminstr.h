// fminstr.h
// Instrument container

#include "fmcore.h"
#include "stddef.h"

typedef uint32_t note_t;

typedef struct fmchstat_s {
  int8_t note;
  int8_t on;
} fmchstat_t;

typedef struct fminstr_s {
  fmsub_t sub;
  size_t n_channels;
  fmchstat_t *ch;
  size_t ch_base;
} fminstr_t;

int fminstr_init(fminstr_t *instr, size_t n_channels /*, patch */);
