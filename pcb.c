#include "pcb.h"

pcb proclist[MAXPROCS];
pcb *currproc = NULL;
pcb *idleproc = NULL;
