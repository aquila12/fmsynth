// fmcore.h
// Common type definitions
#ifndef INCLUDED_FMCORE_H
#define INCLUDED_FMCORE_H

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"

#define RATE 48000

#define PHASE_BITS 32

#define HERTZ ((float)UINT32_MAX / (float)RATE)

typedef uint32_t phase_t;
typedef int32_t frequency_t;

#define SAMPLE_FRACTION 24
#define SAMPLE_1 (1<<SAMPLE_FRACTION)

/* Fixed point multiply */
/* May be faster as a 64-bit mul */
#define MUL_INSHIFT (SAMPLE_FRACTION/2)
#define MUL(s1, s2) ((s1>>MUL_INSHIFT) * (s2>>MUL_INSHIFT))

typedef enum fmev_e {
  fmev_note_off = 0,
  fmev_note_on,
  fmev_freq_change = 16
} fmev_t;

typedef struct fmevent_s {
  fmev_t type;
  union {
    uint8_t note_number;
  }
} fmevent_t;

typedef int32_t sample_t;

int8_t sample_number;

typedef struct fmel_s fmel_t;

typedef void (*fmel_update_func)(fmel_t *el);
typedef void (*fmel_event_func)(fmel_t *el, fmevent_t event);
typedef void (*fmel_cleanup_func)(fmel_t *el);

typedef struct fmel_s {
  union {
    sample_t out;
    frequency_t f;
  };
  fmel_update_func update;
  fmel_event_func event;
  fmel_cleanup_func cleanup;
} fmel_t;

int fmel_init(fmel_t *el);

typedef struct fmcontainer_s {
  fmel_t el;
  fmel_t **p_elements;
  size_t n_elements;
} fmcontainer_t;

int fmcontainer_init(fmcontainer_t *co, size_t n_elements);
//int fmch_init(fmcontainer_t *ch /*, patch */);
int fminstr_init(fmcontainer_t *instr, size_t n_channels /*, patch */);

#endif
