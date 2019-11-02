#include "fmcmd.h"
#include "fmfunc.h"
#include "stdio.h"

void init_test_patch() {
  fmcmd_process("edit");
  fmcmd_process("  clr 1     ; Allocate 1 patch");
  fmcmd_process("  sel 0 32  ; Select patch 0, 32 slots (polyphony)");
  fmcmd_process("  pat 2 2 7 ; Patch def 2 ops, 2 lfo, 7 instructions");
  fmcmd_process("    lfo 0 5.0");
  fmcmd_process("    lfo 1 4.0");
  fmcmd_process("    op.r 0 2.0 ; Operator 0 relative frequency at 2.0*f");
  fmcmd_process("    op.r 1 1.0 ; Operator 1 relative frequency at 1.0*f");
  fmcmd_process("    op.e 1 20.0 10.0 0.2 0.7 3.0 ; Operator 1 envelope");
  fmcmd_process("    prog");
  fmcmd_process("      lmix 0 0.006");
  fmcmd_process("      feed 0");
  fmcmd_process("      mix 0 0.8");
  fmcmd_process("      feed 1");
  fmcmd_process("      mix 1 1.0");
  fmcmd_process("      amod 1 0.2");
  fmcmd_process("      out");
  fmcmd_process("    end");
  fmcmd_process("  end");
  fmcmd_process("end");
}

int main()
{
  fmfunc_setup();

  init_test_patch();

  char cmdline[80];
  cmd_result_t result=cmd_ok;
  while(result != cmd_end) {
    if(fgets(cmdline, sizeof(cmdline), stdin)==0) return -1;
    result = fmcmd_process(cmdline);
    fmcmd_printerr(result, cmdline);
  }

  return 0;
}
