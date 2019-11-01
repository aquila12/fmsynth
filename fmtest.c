#include "fmpatch.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"

#define printerr(...) fprintf (stderr, __VA_ARGS__)

#define BUFSIZE (RATE / 10)
#define GAIN 16

typedef enum cmd_result_e {
  cmd_ok = 0,
  cmd_end,
  cmd_missing,
  cmd_unknown,
  cmd_badparam
} cmd_result_t;

fmpatch_t pat[1];  // The only patch in use right now

void render(int n, fmpatch_t *patch) {
  int16_t outbuf[BUFSIZE];
  int j=0;

  for(int i=0; i<n; ++i) {
    outbuf[j++] = fmpatch_sample(patch) * GAIN >> 16;
    if(j==BUFSIZE) {
      write(1, outbuf, j * sizeof(int16_t)); // Ignoring failed writes
      j=0;
    }
  }
  write(1, outbuf, j * sizeof(int16_t));
}

cmd_result_t process_cmd(const char *cmdline) {
  char cmd[5];
  int i, j, k;

  if(sscanf(cmdline, " %4s", cmd) < 1) return cmd_missing;

  if(strcmp(cmd, "end")==0) {
    return cmd_end;
  } else if(strcmp(cmd, "run")==0) {
    if(sscanf(cmdline, " %*4s %d", &i) < 1) return cmd_badparam;
    render(i, &pat[0]);
    return cmd_ok;
  } else if(strcmp(cmd, "ampl")==0) {
    if(sscanf(cmdline, " %*4s %d %d %d", &i, &j, &k) < 3) return cmd_badparam;
    fmslot_ampl(&pat[i], j, k);
    return cmd_ok;
  } else if(strcmp(cmd, "freq")==0) {
    if(sscanf(cmdline, " %*4s %d %d %d", &i, &j, &k) < 3) return cmd_badparam;
    fmslot_freq(&pat[i], j, k);
    return cmd_ok;
  } else if(strcmp(cmd, "keyd")==0) {
    if(sscanf(cmdline, " %*4s %d %d", &i, &j) < 2) return cmd_badparam;
    fmslot_keydown(&pat[i], j);
    return cmd_ok;
  } else if(strcmp(cmd, "keyu")==0) {
    if(sscanf(cmdline, " %*4s %d %d", &i, &j) < 2) return cmd_badparam;
    fmslot_keyup(&pat[i], j);
    return cmd_ok;
  }

  return cmd_unknown;
}

void print_cmd_err(cmd_result_t e, const char* cmdline) {
  switch(e) {
  case cmd_ok:
    break;
  case cmd_end:
    printerr("End of program reached\n");
    break;
  case cmd_missing:
    printerr("No command on line: %s\n", cmdline);
    break;
  case cmd_unknown:
    printerr("Unknown command: %s\n", cmdline);
    break;
  case cmd_badparam:
    printerr("Incorrect parameters: %s\n", cmdline);
    break;
  default:
    printerr("Unknown error condition (%d)\n", e);
  }
}

void init_test_patch(fmpatch_t *p) {
  fmpatch_alloc(p, 2 /*ops*/, 1 /* lfo */, 32 /* slot */, 6 /* instructions */);

  fmpatch_set_lfo(p, 0, 5.0);

  p->prog[0].opcode = OpLMix;
  p->prog[0].mod.number = 0;
  p->prog[0].mod.gain = 0.006 * SAMPLE_1;
  p->prog[1].opcode = OpFeed;
  p->prog[1].op.number = 0;

  fmop_param_osc_rel(&p->params[0], 2.0);

  p->prog[2].opcode = OpMix;
  p->prog[2].mod.number = 0;
  p->prog[2].mod.gain = 0.8 * SAMPLE_1;
  p->prog[3].opcode = OpFeed;
  p->prog[3].mod.number = 1;

  fmop_param_osc_rel(&p->params[1], 1.0);
  fmop_param_adhr(&p->params[1], 20.0, 10.0, 0.2, 0.7, 3.0);

  p->prog[4].opcode = OpMix;
  p->prog[4].mod.number = 1;
  p->prog[4].mod.gain = SAMPLE_1;
  p->prog[5].opcode = OpOut;
}

int main()
{
  fmfunc_setup();

  init_test_patch(&pat[0]);

/* Patch does this */
/*
 fmosc_init(op0, 2.0, 0.006, &freq->el.f, lfo_out);
 fmosc_init(op1, 1.0, 0.8, &freq->el.f, &op0->el.out);
 fmadhr_init(adhr, 20.0, 10.0, 0.2, 0.7, 3.0, &amp->el.out);
*/

  char cmdline[80];
  cmd_result_t result=cmd_ok;
  while(result != cmd_end) {
    if(fgets(cmdline, sizeof(cmdline), stdin)==0) return -1;
    result = process_cmd(cmdline);
    print_cmd_err(result, cmdline);
  }

  fmpatch_free(&pat[0]);

  return 0;
}
