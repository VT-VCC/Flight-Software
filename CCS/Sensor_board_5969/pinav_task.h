/*
 * pinav_task.h
 *
 *  Created on: Apr 30, 2018
 *      Author: Nick
 */

#ifndef PINAV_TASK_H_
#define PINAV_TASK_H_

#include <hal/uart.h>

/*
 * Initializes the Pinav task
 * Must be called before vTaskStartScheduler() in main
 *
 * Inputs:
 *  None
 * Outputs:
 *  None
 *
 * TODO: Return success/fail
 */
void task_pinav_start(void);

/*
 * Must be called in the receive ISR of the UART module that the pinav is connected to
 * Places the received character in a buffer
 * Once the buffer contains a full pinav sentence, it is enqueued for parsing
 *
 * Inputs:
 *  - chr:  The character received by the pinav UART.
 */
void pinav_accept_char_from_ISR(uint8_t chr);

#endif /* PINAV_TASK_H_ */
