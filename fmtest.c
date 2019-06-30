// fmtest.c
// FM Test program

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"
#include "fmfreq.h"
#include "fminstr.h"

sample_t fm_output;

#define BUFSIZE (RATE / 10)

void render(int n, fmel_t *root) {
  int16_t outbuf[BUFSIZE];
  int j=0;

  for(int i=0; i<n; ++i) {
    fmel_i_update(root);
    outbuf[j++] = (root->out * 32) >> 16;
    if(j==BUFSIZE) {
      write(1, outbuf, j * sizeof(int16_t)); // Ignoring failed writes
      j=0;
    }
  }
  write(1, outbuf, j * sizeof(int16_t));
}

#define NOTEON fmev_note_on
#define NOTEOFF fmev_note_off

uint32_t now = 0;
fmel_t *_root = 0;

void event(uint32_t when, fmev_t what, int8_t channel, int8_t note, int8_t velocity) {
  /* FIXME: Ignored: channel, velocity */
  if(when > now) render(when - now, _root);
  now = when;

  fmel_i_event(_root, (fmevent_t){.type=what, .note_number=note});
}

void canyon() {
#include "ripped-midi.inc"
}

int main() {
  fminstr_t *clar = calloc(1, sizeof(fminstr_t));
  fmel_t *root = &clar->sub.el;
  _root = root;

  fprintf(stderr, "Initializing\n");
  fmosc_setup();
  fmfreq_setup();

  // Initialize 3 channels
  fminstr_init(clar, 3);

  fprintf(stderr, "Initialized\n");

  canyon();

  // uint8_t c4=60, e4=64, g4=67;

  // uint8_t maj_scale[]={0,2,4,5,7,9,11,12,14,16};

  // for(int i=0; i<8; ++i) {
    // clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_on, .note_number=c4-12+maj_scale[i]});
    // clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_on, .note_number=c4-12+maj_scale[i+2]});
    // clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_on, .note_number=c4+maj_scale[7-i]});
    // render(0.9 * 0.25 * RATE, root);
    // clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_off, .note_number=c4-12+maj_scale[i]});
    // clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_off, .note_number=c4-12+maj_scale[i+2]});
    // clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_off, .note_number=c4+maj_scale[7-i]});
    // render(0.1 * 0.25 * RATE, root);
  // }

  fprintf(stderr, "Finished\n");
  return 0;
}
