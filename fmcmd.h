// fmcmd.h
// FM Command Input

typedef enum cmd_result_e {
  cmd_ok = 0,
  cmd_end,
  cmd_missing,
  cmd_unknown,
  cmd_badparam
} cmd_result_t;

typedef cmd_result_t (*cmd_handler_fn)();

cmd_result_t fmcmd_process(const char* cmdline);
void fmcmd_printerr(cmd_result_t e, const char* cmdline);
