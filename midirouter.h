// midirouter.h
// MIDI event router

#include "stdint.h"
#include "fmcore.h"

typedef uint32_t midipacked_t;

typedef union midifilter_u {
  struct {
    uint8_t m_chan;
    uint8_t m_prog;
    uint8_t m_note;
  };
  midipacked_t v;
} midifilter_t;

typedef struct midiroute_s {
  midifilter_t mask, value;
  fmel_t *target;
} midiroute_t;

void mrouter_insert(midifilter_t mask, midifilter_t value, fmel_t *target);
void mrouter_delete(midifilter_t mask, midifilter_t value);
void mrouter_killtarget(fmel_t *target);
void mrouter_compact();
void mrouter_route(midifilter_t input, fmevent_t *event);
