/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"
#include "app_scheduler.h"
#include "pio_it.h"
#include "pio.h"
/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/
static const Pin pinPB = PIN_PUSHBUTTON_0;
/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 *  \brief Handler for Button 1 rising edge interrupt.
 *
 *  Handle process led1 status change.
 */
void _Button_Handler( void )
{
	vfnButton_Handler();
}

/**
 *  \brief Configure the Push-buttons
 *
 *  Configure the PIO as inputs and generate corresponding interrupt when
 *  pressed or released.
 */
void vfnConfigureButtons( void )
{
	/* Configure PIO as inputs. */
	PIO_Configure(&pinPB,1);

	/* Adjust PIO denounce filter parameters, uses 10 Hz filter. */
	PIO_SetDebounceFilter(&pinPB,10);

	/* Initialize PIO interrupt handlers, see PIO definition in board.h. */
	PIO_ConfigureIt(&pinPB, (void*)_Button_Handler ) ; /* Interrupt on rising edge  */

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ(pinPB.id);

	/* Enable PIO line interrupts. */
	PIO_EnableIt(&pinPB);
}





