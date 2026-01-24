#ifndef MAIN_H
#define MAIN_H

//v27.9
//https://github.com/darkwood-byte/os note: prod2 is the main branch!
//deps
#include "deps.h"

//utils
#include "str.h"

//CSR access and trap
#include "csr.h"

#include "trap_k.h"

//Memory management
#include "memory.h"
#include "malloc.h"

//proces mangement
#include "pcb_k.h"
#include "program_k.h"
#include "app.h"

#endif
