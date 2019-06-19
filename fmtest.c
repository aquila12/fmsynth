// fmtest.c
// FM Test program

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"
#include "fmfreq.h"
#include "fminstr.h"

sample_t fm_output;

void render(float duration, fmel_t *root) {
  int n = RATE * duration;
  for(int i=0; i<n; ++i) {
    root->update(root);
    int16_t s = (root->out * 32) >> 16;
    write(1, &s, sizeof(s));
  }
}

int main() {
  fminstr_t *clar = calloc(1, sizeof(fminstr_t));
  fmel_t *root = &clar->sub.el;

  fprintf(stderr, "Initializing\n");
  fmosc_setup();
  fmfreq_setup();

  // Initialize 3 channels
  fminstr_init(clar, 3);

  fprintf(stderr, "Initialized\n");

  uint8_t c4=60, e4=64, g4=67;

  uint8_t maj_scale[]={0,2,4,5,7,9,11,12,14,16};

  for(int i=0; i<8; ++i) {
    clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_on, .note_number=c4-12+maj_scale[i]});
    clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_on, .note_number=c4-12+maj_scale[i+2]});
    clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_on, .note_number=c4+maj_scale[7-i]});
    render(0.9 * 0.25, root);
    clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_off, .note_number=c4-12+maj_scale[i]});
    clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_off, .note_number=c4-12+maj_scale[i+2]});
    clar->sub.el.event(&clar->sub.el, (fmevent_t){.type=fmev_note_off, .note_number=c4+maj_scale[7-i]});
    render(0.1 * 0.25, root);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
