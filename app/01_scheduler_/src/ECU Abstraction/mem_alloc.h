/****************************************
*
*	File: mem_alloc.h
*	Contains all the interfaces provided to the user component modules
*
****************************************/

#include "mem_alloctypes.h"

void Mem_Init(void);
uint32_t* Mem_Alloc(uint16_t);