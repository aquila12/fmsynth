#include "fmpatch.h"
#include "unistd.h"

#define BUFSIZE (RATE / 10)
#define GAIN 32

void render(int n, fmpatch_t *pat) {
  int16_t outbuf[BUFSIZE];
  int j=0;

  for(int i=0; i<n; ++i) {
    outbuf[j++] = fmpatch_sample(pat) * GAIN >> 16;
    if(j==BUFSIZE) {
      write(1, outbuf, j * sizeof(int16_t)); // Ignoring failed writes
      j=0;
    }
  }
  write(1, outbuf, j * sizeof(int16_t));
}

int main()
{
  fmpatch_t pat;

  fmfunc_setup();

  fmpatch_alloc(&pat, 2 /*ops*/, 1 /* lfo */, 2 /* slot */);
  fmpatch_set_lfo(&pat, 0, 5.0);

  fmop_param_osc_rel(&pat.params[0], 2.0);
  fmop_param_osc_rel(&pat.params[1], 1.0);
  fmop_param_adhr(&pat.params[1], 20.0, 10.0, 0.2, 0.7, 3.0);

/* Patch does this */
/*
 fmosc_init(op0, 2.0, 0.006, &freq->el.f, lfo_out);
 fmosc_init(op1, 1.0, 0.8, &freq->el.f, &op0->el.out);
 fmadhr_init(adhr, 20.0, 10.0, 0.2, 0.7, 3.0, &amp->el.out);
*/

  int note[] = {60, 62, 64, 65, 67, 69, 71, 72};

  for(int n=0; n<8; ++n) {
    fmslot_ampl(&pat,0,SAMPLE_1);
    fmslot_freq(&pat,0,note[n] << NOTE_FRACTION);
    fmslot_keydown(&pat,0);
    render(0.2*RATE, &pat);
    fmslot_keyup(&pat,0);
    render(0.05*RATE, &pat);
  }

  fmpatch_free(&pat);

  return 0;
}
