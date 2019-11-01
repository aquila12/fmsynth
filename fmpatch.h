// fmpatch.h
// FM Patch

#include "fmfunc.h"
#include "stddef.h"
#include "fmop.h"

typedef struct fmslot_s {
  fmop_t *op;     /* Operators */
  sample_t ampl;  /* Output amplitude */
  uint8_t active; /* Is this thing on? */
} fmslot_t;

typedef struct fmlfo_s {
  frequency_t f0;
  phase_t ph;
  sample_t value;
} fmlfo_t;

typedef struct fmpatch_s {
  int n_ops, n_lfo, n_slots; /* Component count */
  fmslot_t *slot;       /* Slots */
  fmlfo_t *lfo;         /* LFOs */
  // algorithm
  fmop_param_t *params; /* Slot parameters */
} fmpatch_t;

void fmslot_ampl(fmpatch_t *patch, int index, sample_t ampl);
void fmslot_freq(fmpatch_t *patch, int index, note_t fmnote);
void fmslot_keydown(fmpatch_t *patch, int index);
void fmslot_keyup(fmpatch_t *patch, int index);
sample_t fmpatch_sample(fmpatch_t *patch);

void fmpatch_alloc(fmpatch_t *patch, int ops, int lfos, int slots);
void fmpatch_free(fmpatch_t *patch);

void fmpatch_set_lfo(fmpatch_t *patch, int index, float f0);
