/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.
    Modified by the VT USIP project to compile under the msp430-elf-gcc

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

// For the symbolic vector names

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the MSP430X port.
 *----------------------------------------------------------*/

/* Constants required for hardware setup.  The tick ISR runs off the ACLK,
not the MCLK. */
#define portACLK_FREQUENCY_HZ			( ( TickType_t ) 32768 )
#define portINITIAL_CRITICAL_NESTING	( ( uint16_t ) 10 )
#define portFLAGS_INT_ENABLED			( ( StackType_t ) 0x08 )

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

/* Each task maintains a count of the critical section nesting depth.  Each
time a critical section is entered the count is incremented.  Each time a
critical section is exited the count is decremented - with interrupts only
being re-enabled if the count is zero.

usCriticalNesting will get set to zero when the scheduler starts, but must
not be initialised to zero as this will cause problems during the startup
sequence. */
volatile uint16_t usCriticalNesting = portINITIAL_CRITICAL_NESTING;
/*-----------------------------------------------------------*/


/*
 * Sets up the periodic ISR used for the RTOS tick.  This uses timer 0, but
 * could have alternatively used the watchdog timer or timer 1.
 */
void vPortSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See the header file portable.h.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	uint16_t *pusTopOfStack;
	uint32_t *pulTopOfStack;
	portPOINTER_SIZE_TYPE ulTemp;
	

	/*
		Place a few bytes of known values on the bottom of the stack.
		This is just useful for debugging and can be included if required.
	*/

	*pxTopOfStack = ( StackType_t ) 0x1111;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x2222;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x3333;
	pxTopOfStack--;

	/* Data types are need either 16 bits or 32 bits depending on the data 
	and code model used. */
	if( sizeof( pxCode ) == sizeof( *pxTopOfStack ) )
	{
		/* Here the code and stack data elements are the same size */
		_Static_assert(sizeof(ulTemp) >= sizeof(pxCode),
				"Integer for top of stack is large enough");
		*pxTopOfStack = (StackType_t) pxCode;
		/* We need to write a 16-bit value to the stack for the status register */
		pusTopOfStack = (uint16_t *) pxTopOfStack;
		pusTopOfStack--; // Make room
		*pusTopOfStack = portFLAGS_INT_ENABLED;

		/* And make room for the next item */
		pxTopOfStack = (uint32_t *) pusTopOfStack;
		pxTopOfStack--;
	}
	else if (sizeof(pxCode) == sizeof(uint32_t))
	{
		/* Here we have 20 bit code pointers but a 16 bit stack */
		pusTopOfStack = ( uint16_t * ) pxTopOfStack;
		/* Make room for a 20 bit value stored as a 32 bit value by further
		 * decrementing the top of stack */
		pusTopOfStack--;
		pulTopOfStack = ( uint32_t * ) pusTopOfStack;
		*pulTopOfStack = ( portPOINTER_SIZE_TYPE ) pxCode;
		/* Make room for the next data element on the stack */
		pusTopOfStack--;
		/* Write the initial saved status registers, as a 16 bit value */
		*pusTopOfStack = portFLAGS_INT_ENABLED;
		/* And make room for the next element */
		pusTopOfStack--;
		/* Write back in to pxTopOfStack to share the rest of the code */
		pxTopOfStack = ( StackType_t * ) pusTopOfStack;
	}
	else
	{
		/* Here we have 16 bit code pointers but a 32 bit stack */
		pusTopOfStack = ( uint16_t * ) pxTopOfStack;
		/* Write the return address and the status register state as 16 bit
		 * quantities */
		*pusTopOfStack = ( portPOINTER_SIZE_TYPE ) pxCode;
		pusTopOfStack--;
		*pusTopOfStack = portFLAGS_INT_ENABLED;
		pusTopOfStack--;
		/* Write back in to pxTopOfStack to share the rest of the code */
		pxTopOfStack = ( StackType_t * ) pusTopOfStack;
	}

	
	/* From here on the size of stacked items depends on the memory model. */

	/* Next the general purpose registers. */
	*pxTopOfStack = ( StackType_t ) 0xffff;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xeeee;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xdddd;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xbbbb;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0xaaaa;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x9999;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x8888;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x6666;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x5555;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x4444;
	pxTopOfStack--;

	/* A variable is used to keep track of the critical section nesting.
	This variable has to be stored as part of the task context and is
	initially set to zero. */
	*pxTopOfStack = ( StackType_t ) portNO_CRITICAL_SECTION_NESTING;	

	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the MSP430 port will get stopped.  If required simply
	disable the tick interrupt here. */
}
/*-----------------------------------------------------------*/

/*
 * Hardware initialisation to generate the RTOS tick.
 */
void vPortSetupTimerInterrupt( void )
{
	vApplicationSetupTimerInterrupt();
}
/*-----------------------------------------------------------*/

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void vTickISREntry( void )
{
extern void vPortTickISR( void );
	__bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
	#if configUSE_PREEMPTION == 1
		extern void vPortPreemptiveTickISR( void );
		vPortPreemptiveTickISR();
	#else
		extern void vPortCooperativeTickISR( void );
		vPortCooperativeTickISR();
	#endif
}
