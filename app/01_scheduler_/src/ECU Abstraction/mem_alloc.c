/****************************************
*
*	File: mem_alloc.c
*	Contains the main functionality of the memory allocation handler
*
****************************************/

/*~~~~~~  include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "stdint.h"
#include "mem_alloc.h"
#include "stddef.h"

/*~~~~~~  define ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define mem_alloc_TEST /*comment for disabling the test code for mem_alloc()*/

/*~~~~~~  var ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern uint32_t   _heap_mem_start;
extern uint32_t   _heap_mem_end;
static uint32_t*  lou32mem_alloc_cur_address;

/*~~~~~~  code ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  The basic steps to initialize the heap memory supported by the startup code  */
void Mem_Init(void){
	uint32_t* AddrInit;
	
	for(AddrInit = &_heap_mem_start; AddrInit < &_heap_mem_end; AddrInit++){
		*AddrInit = 0;
	}
  lou32mem_alloc_cur_address = &_heap_mem_start;
}

uint32_t* Mem_Alloc (uint16_t size){
    /*local variables*/
    static uint8_t      lo8free_bytes;
    static uint8_t      lou8size;

    /*local variables for testing the Mem_Alloc*/
#ifdef mem_alloc_TEST
    uint32_t* AddrInit;
    static uint8_t lou8mem_alloc_test_value;
#endif

    /*return as NULL*/
    uint32_t* return_addr = NULL;

    /*calculate free bytes in heap*/
    lo8free_bytes = ( uint8_t) ( ( &_heap_mem_end - lou32mem_alloc_cur_address ) / 8 );

    /*check if reservation size (in bytes) is less or equal to the current free bytes*/
    if (  size <= lo8free_bytes ){

        /*mem alloc test code*/        
#ifdef mem_alloc_TEST
        if (lou8mem_alloc_test_value == 0x00U){
            lou8mem_alloc_test_value++;
        }
        memset(lou32mem_alloc_cur_address, lou8mem_alloc_test_value, (size_t) size);
        lou8mem_alloc_test_value++;
#endif
        /*udpate current address*/
        lou32mem_alloc_cur_address = (uint8_t*)lou32mem_alloc_cur_address + size;

        /*update return address*/
        return_addr = lou32mem_alloc_cur_address;
    }

    return return_addr;
}