// fminstr.c
// Instrument container

#include "fminstr.h"
#include "fmosc.h"
#include "fmamp.h"
#include "stdlib.h"

void fminstr_cleanup(fmel_t *el) {
  fminstr_t *instr=(fminstr_t*)el;

  if(instr->ch) free(instr->ch);
  fmsub_cleanup(el);
}

int fminstr_find_note(fminstr_t *instr, int8_t note_number) {
  for(int i=0; i<instr->n_channels; ++i) {
    if(instr->ch[i].note == note_number) return i;
  }

  return -1;
}

int fminstr_find_off(fminstr_t *instr) {
  for(int i=0; i<instr->n_channels; ++i) {
    if(!instr->ch[i].on) return i;
  }

  return -1;
}

void fminstr_event(fmel_t *el, fmevent_t event) {
  fminstr_t *instr=(fminstr_t*)el;
  int n;
  fmel_t *sel;

  switch(event.type) {
    case fmev_note_on:
    n = fminstr_find_note(instr, event.note_number);
    if(n < 0) n = fminstr_find_off(instr);
    /* TODO: Don't just drop a note-on if we run out of channels */
    if(n < 0) return;
    instr->ch[n].note = event.note_number;
    instr->ch[n].on = 1;
    sel = instr->sub.p_elements[n + instr->ch_base];
    sel->event(sel, event);
    break;

    case fmev_note_off:
    n = fminstr_find_note(instr, event.note_number);

    if(n < 0) return;
    instr->ch[n].on = 0;
    sel = instr->sub.p_elements[n + instr->ch_base];
    sel->event(sel, event);

    default:
    fmsub_event(el, event);
  }
}

int fminstr_init(fminstr_t *instr, size_t n_channels /*, patch */) {
  size_t n_el = 1; /* LFO frequency */
  size_t n_osc = 1; /* "Global" LFOs */
  size_t n_ampl = 1; /* Channel mixer */

  if(fmsub_init(&instr->sub, n_el + n_osc + n_channels + n_ampl)) return -1;

  instr->ch = calloc(n_channels, sizeof(fmchstat_t));
  if(!instr->ch) return -1;
  int n=0;

  /* Patch does this */
  fmel_t *lfo_f = calloc(1, sizeof(fmel_t));
  fmel_init(lfo_f);
  lfo_f->f = 1.5 * HERTZ;

  fmosc_t *osc = calloc(1, sizeof(fmosc_t));
  fmosc_init(osc, 1.0, 0.0, &lfo_f->f, 0);

  fmel_t **el = instr->sub.p_elements;
  el[n++] = lfo_f;
  el[n++] = &osc->el;
  fmamp_t *amp = calloc(1, sizeof(fmamp_t));
  if(fmamp_init(amp, n_channels)) return -1;

  fmsub_t *ch;
  instr->ch_base = n;
  for(int i=0; i<n_channels; ++i) {
    ch = calloc(1, sizeof(fmsub_t));
    if(fmch_init(ch, &osc->el.out)) return -1;
    fmamp_connect(amp, i, &ch->el.out, 1.0);
    el[n++] = &ch->el;
  }
  el[n++] = &amp->el;

  return 0;
}
