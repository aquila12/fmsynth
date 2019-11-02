// fmcmd.c
// FM Command Input

#include "fmcmd.h"
#include "fmrender.h"
#include "stdio.h"
#include "string.h"

#define printerr(...) fprintf (stderr, __VA_ARGS__)

static const char *cmdline;
static char current_cmd[5];
static cmd_result_t result;
static enum fmcmd_mode_e {
  fmcmd_default = 0,
  fmcmd_edit,
  fmcmd_pat,
  fmcmd_prog
} mode;
static int sel_patch = 0;
static int sel_poly = 1;
static int prog_i = 0;

static void dispatch(const char* cmd, cmd_handler_fn handler) {
  if(strcmp(current_cmd, cmd)==0) result = handler();
}

/* Default mode */
static cmd_result_t handle_end() {
  if(patch) fmrender_free();
  return cmd_end;
}

static cmd_result_t handle_run() {
  int n;
  if(sscanf(cmdline, " %*4s %d", &n) < 1) return cmd_badparam;
  fmrender_samples(n);
  return cmd_ok;
}

static cmd_result_t handle_ampl() {
  int p, s, a;
  if(sscanf(cmdline, " %*4s %d %d %d", &p, &s, &a) < 3) return cmd_badparam;
  fmslot_ampl(&patch[p], s, a);
  return cmd_ok;
}

static cmd_result_t handle_freq() {
  int p, s, f;
  if(sscanf(cmdline, " %*4s %d %d %d", &p, &s, &f) < 3) return cmd_badparam;
  fmslot_freq(&patch[p], s, f);
  return cmd_ok;
}

static cmd_result_t handle_keyd() {
  int p, s;
  if(sscanf(cmdline, " %*4s %d %d", &p, &s) < 2) return cmd_badparam;
  fmslot_keydown(&patch[p], s);
  return cmd_ok;
}

static cmd_result_t handle_keyu() {
  int p, s;
  if(sscanf(cmdline, " %*4s %d %d", &p, &s) < 2) return cmd_badparam;
  fmslot_keyup(&patch[p], s);
  return cmd_ok;
}

static cmd_result_t handle_edit() {
  mode = fmcmd_edit;
  return cmd_ok;
}

/* Edit commands */
static cmd_result_t handle_edit_end() {
  mode = fmcmd_default;
  return cmd_ok;
}

static cmd_result_t handle_edit_clr() {
  int n;
  if(sscanf(cmdline, " %*4s %d", &n) < 1) return cmd_badparam;
  if(patch) fmrender_free();
  fmrender_alloc(n);
  return cmd_ok;
}

static cmd_result_t handle_edit_sel() {
  if(sscanf(cmdline, " %*4s %d %d", &sel_patch, &sel_poly) < 2) return cmd_badparam;
  return cmd_ok;
}

static cmd_result_t handle_edit_pat() {
  int o,l,i;
  if(sscanf(cmdline, " %*4s %d %d %d", &o, &l, &i) < 3) return cmd_badparam;
  fmpatch_alloc(&patch[sel_patch], o, l, sel_poly, i);
  mode = fmcmd_pat;
  return cmd_ok;
}

/* Patch commands */
static cmd_result_t handle_pat_end() {
  mode = fmcmd_edit;
  return cmd_ok;
}

static cmd_result_t handle_pat_lfo() {
  int l;
  float freq;
  if(sscanf(cmdline, " %*4s %d %f", &l, &freq) < 2) return cmd_badparam;
  fmpatch_set_lfo(&patch[sel_patch], l, freq);
  return cmd_ok;
}

static cmd_result_t handle_pat_op_r() {
  int o;
  float fmul;
  if(sscanf(cmdline, " %*4s %d %f", &o, &fmul) < 2) return cmd_badparam;
  fmop_param_osc_rel(&patch[sel_patch].params[o], fmul);
  return cmd_ok;
}

static cmd_result_t handle_pat_op_a() {
  int o;
  float freq;
  if(sscanf(cmdline, " %*4s %d %f", &o, &freq) < 2) return cmd_badparam;
  fmop_param_osc_abs(&patch[sel_patch].params[o], freq);
  return cmd_ok;
}

static cmd_result_t handle_pat_op_e() {
  int o;
  float a, d, h, h_level, r;
  if(sscanf(cmdline, " %*4s %d %f %f %f %f %f", &o, &a, &d, &h, &h_level, &r) < 6) return cmd_badparam;
  fmop_param_adhr(&patch[sel_patch].params[o], a, d, h, h_level, r);
  return cmd_ok;
}

static cmd_result_t handle_pat_prog() {
  mode = fmcmd_prog;
  prog_i = 0;
  return cmd_ok;
}

/* Prog commands */
static cmd_result_t handle_prog_end() {
  mode = fmcmd_pat;
  return cmd_ok;
}

static cmd_result_t _op_set_params() {
  int number;
  if(sscanf(cmdline, " %*4s %d", &number) < 1) return cmd_badparam;
  patch[sel_patch].prog[prog_i].op.number = number;
  return cmd_ok;
}

static cmd_result_t _mod_set_params() {
  int number;
  float gain;
  if(sscanf(cmdline, " %*4s %d %f", &number, &gain) < 2) return cmd_badparam;
  patch[sel_patch].prog[prog_i].mod.number = number;
  patch[sel_patch].prog[prog_i].mod.gain = gain * SAMPLE_1;
  return cmd_ok;
}

static cmd_result_t handle_prog_out() {
  patch[sel_patch].prog[prog_i].opcode = OpOut;
  return cmd_ok;
}

static cmd_result_t handle_prog_feed() {
  patch[sel_patch].prog[prog_i].opcode = OpFeed;
  return _op_set_params();
}

static cmd_result_t handle_prog_mix() {
  patch[sel_patch].prog[prog_i].opcode = OpMix;
  return _mod_set_params();
}

static cmd_result_t handle_prog_lmix() {
  patch[sel_patch].prog[prog_i].opcode = OpLMix;
  return _mod_set_params();
}

static cmd_result_t handle_prog_amod() {
  patch[sel_patch].prog[prog_i].opcode = OpAMod;
  return _mod_set_params();
}


cmd_result_t fmcmd_process(const char* line) {
  cmdline = line;
  result = cmd_unknown;
  if(sscanf(cmdline, " %4s", current_cmd) < 1) return cmd_missing;

  switch(mode) {
  case fmcmd_default:
    dispatch("end", handle_end);
    dispatch("run", handle_run);
    dispatch("ampl", handle_ampl);
    dispatch("freq", handle_freq);
    dispatch("keyd", handle_keyd);
    dispatch("keyu", handle_keyu);
    dispatch("edit", handle_edit);
    break;
  case fmcmd_edit:
    dispatch("end", handle_edit_end);
    dispatch("clr", handle_edit_clr);
    dispatch("sel", handle_edit_sel);
    dispatch("pat", handle_edit_pat);
    break;
  case fmcmd_pat:
    dispatch("end", handle_pat_end);
    dispatch("lfo", handle_pat_lfo);
    dispatch("op.r", handle_pat_op_r);
    dispatch("op.a", handle_pat_op_a);
    dispatch("op.e", handle_pat_op_e);
    dispatch("prog", handle_pat_prog);
    break;
  case fmcmd_prog:
    dispatch("end", handle_prog_end);
    dispatch("out", handle_prog_out);
    dispatch("feed", handle_prog_feed);
    dispatch("mix", handle_prog_mix);
    dispatch("lmix", handle_prog_lmix);
    dispatch("amod", handle_prog_amod);
    if(result == cmd_ok) ++prog_i;
    break;
  }

  return result;
}

void fmcmd_printerr(cmd_result_t e, const char* line) {
  switch(e) {
  case cmd_ok:
    break;
  case cmd_end:
    printerr("End of program reached\n");
    break;
  case cmd_missing:
    printerr("No command on line: %s\n", line);
    break;
  case cmd_unknown:
    printerr("Unknown command: %s\n", line);
    break;
  case cmd_badparam:
    printerr("Incorrect parameters: %s\n", line);
    break;
  default:
    printerr("Unknown error condition (%d)\n", e);
  }
}
