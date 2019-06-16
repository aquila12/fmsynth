// fmcore.h
// Common type definitions
#ifndef INCLUDED_FMCORE_H
#define INCLUDED_FMCORE_H

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"

#define RATE 48000

typedef enum fmevent_e {
  fmev_note_off = 0,
  fmev_note_on,
  fmev_freq_change = 16
} fmevent_t;

typedef int32_t sample_t;

int8_t sample_number;

typedef struct fmel_s fmel;
typedef sample_t (*fmel_update_func)(fmel *el);
typedef void (*fmel_event_func)(fmel *el, fmevent_t event, const void *event_data);

typedef struct fmel_s {
  fmel_update_func update;
  fmel_event_func event;
  sample_t out;
  int8_t last_updated;
  //void *data;
} fmel;

sample_t fmel_resolve(fmel *el);

#endif
