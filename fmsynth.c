#include "fmcmd.h"
#include "fmfunc.h"
#include "stdio.h"

int main()
{
  fmfunc_setup();

  char cmdline[80];
  cmd_result_t result=cmd_ok;
  while(result != cmd_end) {
    if(fgets(cmdline, sizeof(cmdline), stdin)==0) return -1;
    result = fmcmd_process(cmdline);
    fmcmd_printerr(result, cmdline);
  }

  return 0;
}
