/*
 * pinav_task.c
 *
 *  Created on: Apr 30, 2018
 *      Author: Nick
 */

#include "pinav_task.h"
#include "pinav_parser.h"
#include "global_defines.h"

/* FreeRTOS include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define PINAV_TASK_PRIORITY 1   //  The pinav task's priority level within FreeRTOS

#define PINAV_SENTENCE_QUEUE_LEN 4                              //  The length of the queue which holds sentences to be parsed
#define PINAV_SENTENCE_QUEUE_ITEM_SIZE PINAV_MAX_SENTENCE_LEN   //  The size of an item in the pinav sentence queue is the
                                                                //  maximum possible pinav sentence length
#define PINAV_TASK_STACK_SIZE 1024

static QueueHandle_t PERSISTENT pinav_sentence_queue_handle;    //  Handle for the queue which holds sentences to be parsed
static StaticQueue_t PERSISTENT pinav_sentence_queue;           //  Queue which holds sentences to be parsed
                                                                //  Sentences are enqueued from the uart receive interrupt handler
static uint8_t PERSISTENT pinav_sentence_queue_storage[PINAV_SENTENCE_QUEUE_LEN * PINAV_SENTENCE_QUEUE_ITEM_SIZE];  // Storage space for the pinav sentence queue
static uint8_t pinav_parse_buffer[PINAV_MAX_SENTENCE_LEN] = {0}; //  Buffer holding the pinav sentence currently being parsed
                                                                            //  Sentences are dequeued from pinav_sentence_queue for parsing

// Buffer which holds data being read from the pinav
static uint8_t pinav_read_buffer[PINAV_MAX_SENTENCE_LEN] = {0};
static size_t pinav_read_buffer_index = 0;

// FreeRTOS variables for the pinav task
static TaskHandle_t PERSISTENT pinav_task_handle;
StaticTask_t PERSISTENT pinav_task;
StackType_t PERSISTENT pinav_task_stack[PINAV_TASK_STACK_SIZE];

void task_pinav(void * params); // The pinav task function, run by the FreeRTOS scheduler

static pinav_parse_output_t PERSISTENT pinav_parsed_sentence = {0}; // Contains the most recently parsed pinav data

// Setup the pinav task
void task_pinav_start(void){
    // Setup queue for received pinav sentences
    pinav_sentence_queue_handle = xQueueCreateStatic(
        PINAV_SENTENCE_QUEUE_LEN,
        PINAV_SENTENCE_QUEUE_ITEM_SIZE,
        pinav_sentence_queue_storage,
        &pinav_sentence_queue);
    if(!pinav_sentence_queue_handle){
        DEBUG("Failed to create pinav sentence queue");
    }

    // Create the pinav task within FreeRTOS
    pinav_task_handle = xTaskCreateStatic(
            task_pinav,
            "pinav",
            PINAV_TASK_STACK_SIZE,
            NULL,
            PINAV_TASK_PRIORITY,
            pinav_task_stack,
            &pinav_task
        );
    if(!pinav_task_handle){
        DEBUG("Failed to create pinav task");
    }
}

/*
 * Dequeues pinav sentences out of the pinav_sentence_queue,
 *  which is fed by pinav_accept_char_from_ISR()
 * Parses the GPS data from sentences into a struct defined in pinav_parser.h
 */
void task_pinav(void * params) {
    for(;;) {
        // Dequeue a message if available
        if (uxQueueMessagesWaiting(pinav_sentence_queue_handle) > 0){
            if(xQueueReceive(pinav_sentence_queue_handle, pinav_parse_buffer, (TickType_t) 0) != pdTRUE){
                DEBUG("Failed to receive from pinav sentence queue");
            }

            // Parse the data out of the received pinav sentence
            pinav_parser_status_t parse_result = parse_pinav_sentence(&pinav_parsed_sentence, pinav_parse_buffer);

            // TODO: the below is for testing; needs to be replaced with flight code for handling pinav data
            switch(parse_result){
                case PN_PARSE_OK:
                    uart_write_string(&standard_output, "PN_PARSE_OK");
                    break;
                case PN_PARSE_NULL_OUTPUT_PTR:
                    uart_write_string(&standard_output, "PN_PARSE_NULL_OUTPUT_PTR");
                    break;
                case PN_PARSE_NULL_SENTENCE_PTR:
                    uart_write_string(&standard_output, "PN_PARSE_NULL_SENTENCE_PTR");
                    break;
                case PN_PARSE_UNRECOGNIZED_SENTENCE_TYPE:
                    uart_write_string(&standard_output, "PN_PARSE_UNRECOGNIZED_SENTENCE_TYPE");
                    break;
                case PN_PARSE_IMPROPER_SENTENCE_LENGTH:
                    uart_write_string(&standard_output, "PN_PARSE_IMPROPER_SENTENCE_LENGTH");
                    break;
                case PN_PARSE_CHECKSUM_FAILURE:
                    uart_write_string(&standard_output, "PN_PARSE_CHECKSUM_FAILURE");
                    break;
                case PN_PARSE_SENTENCE_FORMAT_ERROR:
                    uart_write_string(&standard_output, "PN_PARSE_SENTENCE_FORMAT_ERROR");
                    break;
                case PN_PARSE_UNEXPECTED_UNIT_ENCOUNTERED:
                    uart_write_string(&standard_output, "PN_PARSE_UNEXPECTED_UNIT_ENCOUNTERED");
                    break;
            }
        }
    }
}

/*
 * buffers bytes read from uart into a sentence
 * enqueues complete sentence once \n is received
 */
void pinav_accept_char_from_ISR(uint8_t chr){

    // Flush buffer if a new pinav sentence has been started
    // All pinav sentences begin with '$'
    if (chr == '$'){
        if(xQueueSendToBackFromISR(pinav_sentence_queue_handle, pinav_read_buffer, (TickType_t) 10) != pdTRUE){
            DEBUG("Pinav message enqueue failed");
        }
        pinav_read_buffer_index = 0;
        // TODO: zero out buffer here?
    }

    // Place character in pinav read buffer
    pinav_read_buffer[pinav_read_buffer_index] = chr;
    pinav_read_buffer_index++;

    // Flush buffer if a sentence has been completed
    // All pinav sentences end with '\n'
    if (chr == '\n' || pinav_read_buffer_index >= PINAV_MAX_SENTENCE_LEN){
        if(xQueueSendToBackFromISR(pinav_sentence_queue_handle, pinav_read_buffer, (TickType_t) 10) != pdTRUE){
            DEBUG("Pinav message enqueue failed");
        }
        pinav_read_buffer_index = 0;
        // TODO: zero out buffer here?
    }
}
