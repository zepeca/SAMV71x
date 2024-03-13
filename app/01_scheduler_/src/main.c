/****************************************************************************************************/
/**
  \mainpage
  \n 
  \brief        Main application (main module)
  \author       Abraham Tezmol Otero, M.S.E.E
  \project      Tau 
  \version      1.0
  \date         12/Jun/2016
   
  Program compiled with  WinIDEA Open Build 9.12.256 (54401), tested on Atmel SAMV71 Xplained Ultra
*/
/****************************************************************************************************/


/*~~~~~~  Headers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/** Main group of includes for board definitions, chip definitions and type definitions */
#include    "board.h"
/** Task scheduler definitions */
#include    "app_scheduler.h"
/** LED control definitions */ 
#include    "ECU Abstraction/led_ctrl.h" 
/** SW control definitions */ 
#include    "ECU Abstraction/sw_ctrl.h" 
/** Memory allocation definitions */ 
#include    "ECU Abstraction/mem_alloc.h"

#include    "chip.h"

#include "stdint.h"

/*~~~~~~  Local definitions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~  Global variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~  Local functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 *  \brief getting-started Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
extern int main( void )
{
	uint32_t* AddrActual = 0;

  /* Disable watchdog */
	//vfnWdtCtrl_Disable();
	WDT_Disable( WDT ) ;
  
	/* Enable I and D cache */
	SCB_EnableICache();
	/* SCB_EnableDCache(); */

	/* Configure LEDs */
	vfnLedCtrl_Configure(); 

	/*configure buttons*/
  	vfnConfigureButtons() ;

	/* memory reservation*/
	AddrActual = Mem_Alloc(5);
	AddrActual = Mem_Alloc(10);
  AddrActual = Mem_Alloc(8);
	AddrActual = Mem_Alloc(30);
	AddrActual = Mem_Alloc(1000);

	/* Configure Non-preemtive scheduler */
	vfnScheduler_Init();
	/* Start scheduler */
	vfnScheduler_Start();
	
	/* Once all the basic services have been started, go to infinite loop to serviced activated tasks */
	for(;;)
    {
		vfnTask_Scheduler();
	}
}
