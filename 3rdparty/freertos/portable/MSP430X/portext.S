#    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
#    All rights reserved
#
#
#    ***************************************************************************
#     *                                                                       *
#     *    FreeRTOS tutorial books are available in pdf and paperback.        *
#     *    Complete, revised, and edited pdf reference manuals are also       *
#     *    available.                                                         *
#     *                                                                       *
#     *    Purchasing FreeRTOS documentation will not only help you, by       *
#     *    ensuring you get running as quickly as possible and with an        *
#     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
#     *    the FreeRTOS project to continue with its mission of providing     *
#     *    professional grade, cross platform, de facto standard solutions    *
#     *    for microcontrollers - completely free of charge!                  *
#     *                                                                       *
#     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
#     *                                                                       *
#     *    Thank you for using FreeRTOS, and thank you for your support!      *
#     *                                                                       *
#    ***************************************************************************
#
#
#    This file is part of the FreeRTOS distribution.
#    Modified from the MSP430X port to build under msp430-elf-as
#
#    FreeRTOS is free software; you can redistribute it and/or modify it under
#    the terms of the GNU General Public License (version 2) as published by the
#    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
#    >>>NOTE<<< The modification to the GPL is included to allow you to
#    distribute a combined work that includes FreeRTOS without being obliged to
#    provide the source code for proprietary components outside of the FreeRTOS
#    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#    more details. You should have received a copy of the GNU General Public
#    License and the FreeRTOS license exception along with FreeRTOS; if not it
#    can be viewed here: http://www.freertos.org/a00114.html and also obtained
#    by writing to Richard Barry, contact details for whom are available on the
#    FreeRTOS WEB site.
#
#    1 tab == 4 spaces!
#
#    http://www.FreeRTOS.org - Documentation, latest information, license and
#    contact details.
#
#    http://www.SafeRTOS.com - A version that is certified for use in safety
#    critical systems.
#
#    http://www.OpenRTOS.com - Commercial support, development, porting,
#    licensing and training services.

  .global xTaskIncrementTick
  .global vTaskSwitchContext
  .global vPortSetupTimerInterrupt
  .global pxCurrentTCB
  .global usCriticalNesting

  .global vPortPreemptiveTickISR
  .type vPortPreemptiveTickISR, @function
  .global vPortCooperativeTickISR
  .type vPortCooperativeTickISR, @function
  .global vPortYield
  .type vPortYield, @function
  .global xPortStartScheduler
  .type xPortStartScheduler, @function

#-----------------------------------------------------------

# Saves the current execution context
.macro portSAVE_CONTEXT
  # Save all the general purpose registers
  pushm.a #12, r15
  # Save the current critical section nesting
  mov.w usCriticalNesting, r14
  pushx.a r14
  # Save the current stack pointer to the task control block
  mova pxCurrentTCB, r12
  mova r1, @r12
  .endm
#-----------------------------------------------------------

# restores a previously saved execution context
.macro portRESTORE_CONTEXT
  # Restore the stack pointer
  mova pxCurrentTCB, r12
  mova @r12, r1
  # Restore the current critical section nesting
  popx.a r15
  mov.w r15, &usCriticalNesting
  # Restore general purpose registers
  popm.a #12, r15
  # Restore status flags and return in to user code
  nop
  pop r2
  nop
  reta
  .endm
#-----------------------------------------------------------

#*
#* The RTOS tick ISR.
#*
#* If the cooperative scheduler is in use this simply increments the tick
#* count.
#*
#* If the preemptive scheduler is in use a context switch can also occur.
#*/

  .text
  .align 2

vPortPreemptiveTickISR:
  # Save the sr. Needs to be done out here so vPortYield can be special
  push r2
  portSAVE_CONTEXT

  call.a  #xTaskIncrementTick
  call.a  #vTaskSwitchContext

  portRESTORE_CONTEXT
#-----------------------------------------------------------

  .align 2

vPortCooperativeTickISR:
  # Save the sr. Needs to be done out here so vPortYield can be special
  push r2
  portSAVE_CONTEXT

  call.a  #xTaskIncrementTick

  portRESTORE_CONTEXT

#-----------------------------------------------------------

#
# Manual context switch called by the portYIELD() macro.
#

  .align 2

vPortYield:
  # The sr needs saving before it is modified.
  push  r2

  # Now the SR is stacked we can disable interrupts.
  dint
  nop

  # Save the context of the current task.
  portSAVE_CONTEXT

  # Select the next task to run.
  call.a  #vTaskSwitchContext

  # Restore the context of the new task.
  portRESTORE_CONTEXT
#-----------------------------------------------------------


#
# Start off the scheduler by initialising the RTOS tick timer, then restoring
# the context of the first task.
#

  .align 2

xPortStartScheduler:

  # Setup the hardware to generate the tick.  Interrupts are disabled
  # when this function is called.
  call.a  #vPortSetupTimerInterrupt

  # Restore the context of the first task that is going to run.
  portRESTORE_CONTEXT
#-----------------------------------------------------------

  .end

