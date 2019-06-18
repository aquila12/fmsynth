// fmtest.c
// FM Test program

#include "stdlib.h"
#include "unistd.h"
#include "fmcore.h"
#include "fmosc.h"

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
  fmcontainer_t *clar = calloc(1, sizeof(fmcontainer_t));
  fmel_t *root = &clar->el;

  fprintf(stderr, "Initializing\n");
  fmosc_setup();

  // Initialize 3 channels
  fminstr_init(clar, 3);

  fprintf(stderr, "Initialized\n");

  const float c4 = 261.6256;
  const float e4 = 329.6276;
  const float g4 = 391.9954;

  for(int i=0; i<3; ++i) {
    clar->el.event(&clar->el, fmev_note_on, 0);
    clar->p_elements[0]->event(clar->p_elements[0], fmev_freq_change, &c4);
    clar->p_elements[1]->event(clar->p_elements[1], fmev_freq_change, &e4);
    clar->p_elements[2]->event(clar->p_elements[2], fmev_freq_change, &g4);
    render(5.0 * 0.25, root);
    clar->el.event(&clar->el, fmev_note_off, 0);
    render(1.0 * 0.25, root);
  }

  fprintf(stderr, "Finished\n");
  return 0;
}
