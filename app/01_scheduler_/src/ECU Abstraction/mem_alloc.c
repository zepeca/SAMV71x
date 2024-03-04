/****************************************
*
*	File: mem_alloc.c
*	Contains the main functionality of the memory allocation handler
*
****************************************/
#include "stdint.h"
#include "mem_alloc.h"

extern uint32_t _heap_mem_start;
extern uint32_t _heap_mem_end;

/*  The basic steps to initialize the heap memory supported by the startup code  */
void Mem_Init(void){
	uint32_t* AddrInit;

	
	for(AddrInit = &_heap_mem_start; AddrInit < &_heap_mem_end; AddrInit++){
		*AddrInit = 0;
	}
}


MemReturnType Mem_Alloc(MemSizeType size){
	MemReturnType AddrActual;

    MemHandlerType MemControl = 
    { 
        .MemStart = (uint8_t *) &_heap_mem_start, /* Sets the start of the heap memory */ 
        .MemEnd = (uint8_t *) &_heap_mem_end, /* Sets the end of the heap memory */ 
        .CurrAddr = (uint8_t *) &_heap_mem_start, /* Initialize the current start address */ 
        .FreeBytes = (uint8_t *) &_heap_mem_end - (uint8_t *) &_heap_mem_start, /* Sets the size of the heap memory */ 
    };
    /* Mem_Alloc to do: Condition to fill mem*/
};