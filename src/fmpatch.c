// fmpatch.c
// FM Patch

#include "fmpatch.h"
#include "stdlib.h"

void fmslot_ampl(fmpatch_t *patch, int index, sample_t ampl) {
  patch->slot[index].ampl = ampl;
}

void fmslot_freq(fmpatch_t *patch, int index, note_t fmnote) {
  frequency_t note_freq = frequency(fmnote);
  fmop_t *op;
  fmop_param_t *params;
  for(int i=0; i<patch->n_ops; ++i) {
    op = &patch->slot[index].op[i];
    params = &patch->params[i];
    if(params->abs_freq) op->f0 = params->f0;
    else op->f0 = MUL(note_freq, params->freq_mul);
  }
}

void fmslot_keydown(fmpatch_t *patch, int index) {
  for(int i=0; i<patch->n_ops; ++i)
    patch->slot[index].op[i].mode = adhr_attack;

  patch->slot[index].active = 1;
}

void fmslot_keyup(fmpatch_t *patch, int index) {
  for(int i=0; i<patch->n_ops; ++i)
    patch->slot[index].op[i].mode = adhr_release;
}

void fmlfo_update(fmlfo_t *lfo) {
  lfo->ph += lfo->f0;
  lfo->value = sine(lfo->ph);
}

sample_t fmslot_sample(fmpatch_t *patch, int index) {
  fmslot_t *slot = &patch->slot[index];
  int active = 0;
  sample_t signal = 0;
  int i;

  for(fminstruction_t *ins=patch->prog; ins->opcode != OpOut; ++ins) {
    switch(ins->opcode) {
    case OpFeed:
      i = ins->op.number;
      fmop_update(&slot->op[i], &patch->params[i], signal);
      signal = 0;
      active = 0;
      break;
    case OpMix:
      i = ins->mod.number;
      signal += MUL(ins->mod.gain, slot->op[i].value);
      if(slot->op[i].mode != adhr_idle) active = 1;
      break;
    case OpLMix:
      i = ins->mod.number;
      signal += MUL(ins->mod.gain, patch->lfo[i].value);
      break;
    case OpAMod:
      i = ins->mod.number;
      signal = MUL(signal, SAMPLE_1 + MUL(ins->mod.gain, patch->lfo[i].value));
      break;
    }
  }

  //if(!active) slot->active = 0;
  return MUL(signal, slot->ampl);
}

sample_t fmpatch_sample(fmpatch_t *patch) {
  for(int i=0; i<patch->n_lfo; ++i) fmlfo_update(&patch->lfo[i]);

  sample_t output = 0;
  for(int i=0; i<patch->n_slots; ++i) {
    if(!patch->slot[i].active) continue;
    output += fmslot_sample(patch, i);
  }
  return output;
}

void fmpatch_alloc(fmpatch_t *patch, int ops, int lfos, int slots, int prog_size) {
  int total_operators = ops * slots;

  patch->n_slots = slots;

  patch->n_ops = ops;
  patch->slot = calloc(slots, sizeof(fmslot_t));
  patch->params = calloc(ops, sizeof(fmop_param_t));

  patch->n_lfo = lfos;
  patch->lfo = calloc(lfos, sizeof(fmlfo_t));

  patch->prog_size = prog_size;
  patch->prog = calloc(prog_size, sizeof(fminstruction_t));

  fmop_t *op_ptr = calloc(total_operators, sizeof(fmop_t));
  fmslot_t *slot;

  for(int i=0; i<slots; ++i) {
    slot = &patch->slot[i];
    slot->op = op_ptr;
    op_ptr += ops;
  }
}

void fmpatch_free(fmpatch_t *patch) {
  free(patch->slot[0].op); /* Frees all ops */
  free(patch->params);
  free(patch->slot);
  free(patch->lfo);
}

void fmpatch_set_lfo(fmpatch_t *patch, int index, float f0) {
  patch->lfo[index].f0 = f0 * HERTZ;
}
