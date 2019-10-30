// fmalgo.h
// Program algorithm

#include "fmcore.h"
#include "stddef.h"

typedef enum fmalgoinstr_e {
  Op,     /* Commit buffer to operator modulation input */
  Mix,    /* Mix in operator to buffer with gain */
  Out
} fmalgoinstr_t;

typedef struct fmalgostep_s {
  fmalgoinstr_t instr;
  union fmalgoparam_u {
    struct fmalgoparam_op_s {
      int op;         /* Operator number */
    } op;
    struct fmalgoparam_mod_s {
      int operator;   /* LFO or Operator number */
      sample_t beta;  /* Modulation Index */
    } mix;
  } param;
} fmalgostep_t;
