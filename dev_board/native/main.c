#include <msp430.h>
#include <driverlib.h>


/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "uart.h"
#include "pinav_parser.h"

#define PERSISTENT __attribute__((section(".persistent")))

/******************************************************************************\
 *  Static variables                                                          *
\******************************************************************************/

/// Standard UART output
static uart_t standard_output;

/// LED flash queue
#define BLINK_QUEUE_LENGTH 8
typedef uint16_t blink_queue_item_t;
static QueueHandle_t PERSISTENT blink_queue_handle;
static StaticQueue_t PERSISTENT blink_queue;
static uint8_t PERSISTENT blink_queue_storage[BLINK_QUEUE_LENGTH * sizeof(blink_queue_item_t)];

/******************************************************************************\
 *  Private functions                                                         *
\******************************************************************************/
/// Configures I/O pins
static void hardware_config();
/// Flasshes LEDs if the ACLK is configured at the expected frequency
static void test_aclk();

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

static TaskHandle_t PERSISTENT blink_led_task;
void task_blink_led_start();
void task_blink_led(void * params);

static TaskHandle_t PERSISTENT transmit_blink_signal_task;
void task_pinav_start();
void task_pinav(void * params);

void pinav_accept_char_from_ISR(uint8_t chr); // buffers bytes read from uart into a sentence

/*
 * Pinav task data
 */
uint8_t pinav_read_buffer[PINAV_MAX_SENTENCE_LEN] = {0};
size_t pinav_read_buffer_index = 0;
#define PINAV_SENTENCE_QUEUE_LEN 4
#define PINAV_SENTENCE_QUEUE_ITEM_SIZE PINAV_MAX_SENTENCE_LEN
static QueueHandle_t pinav_sentence_queue_handle;
static StaticQueue_t pinav_sentence_queue;
static uint8_t pinav_sentence_queue_storage[PINAV_SENTENCE_QUEUE_LEN * PINAV_SENTENCE_QUEUE_ITEM_SIZE];
uint8_t pinav_parse_buffer[PINAV_MAX_SENTENCE_LEN] = {0};
pinav_parse_output_t pinav_parsed_sentence = {0};

/******************************************************************************\
 *  Function implementations                                                  *
\******************************************************************************/
int main(void) {

    hardware_config();

    P4OUT |= 0xFF;
    P1OUT |= 0xFF;

    test_aclk();

    P1OUT |= 0xFF;

    uart_open(EUSCI_A0, BAUD_9600, &standard_output);

    blink_queue_handle = xQueueCreateStatic(
            BLINK_QUEUE_LENGTH,
            sizeof(blink_queue_item_t),
            blink_queue_storage,
            &blink_queue);
	
	// Setup queue for pinav sentences
	pinav_sentence_queue_handle = xQueueCreateStatic(
		PINAV_SENTENCE_QUEUE_LEN,
		PINAV_SENTENCE_QUEUE_ITEM_SIZE,
		pinav_sentence_queue_storage,
		&pinav_sentence_queue);

    task_blink_led_start();
    task_pinav_start();

    uart_write_string(&standard_output, "Tasks initialized, starting scheduler\n");

    P1OUT ^= 1 << 6;
    vTaskStartScheduler();

    // there is no way to get here since we are using statically allocated
    // kernel structures

    return 0;
}

static void hardware_config() {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    // Set all GPIO pins to output low for low power
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7|GPIO_PIN8|GPIO_PIN9|GPIO_PIN10|GPIO_PIN11|GPIO_PIN12|GPIO_PIN13|GPIO_PIN14|GPIO_PIN15);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7|GPIO_PIN8|GPIO_PIN9|GPIO_PIN10|GPIO_PIN11|GPIO_PIN12|GPIO_PIN13|GPIO_PIN14|GPIO_PIN15);
    // Configure UCA0RXD for input
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN1, GPIO_SECONDARY_MODULE_FUNCTION);
    // Configure UCA0TXD for output
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0, GPIO_SECONDARY_MODULE_FUNCTION);

    // Configure GPIO to use LFXT
    GPIO_setAsPeripheralModuleFunctionInputPin(
           GPIO_PORT_PJ,
           GPIO_PIN4 + GPIO_PIN5,
           GPIO_PRIMARY_MODULE_FUNCTION
           );

    // Set DCO frequency to 8 MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);
    //Set external clock frequency to 32.768 KHz
    CS_setExternalClockSource(32768, 0);
    //Set ACLK=LFXT
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //Start XT1 with no time out
    CS_turnOnLFXT(CS_LFXT_DRIVE_0);

    __enable_interrupt();
}

static void test_aclk() {
    if (CS_getACLK() != 32768) {
        // Flash red LED if clock is bad
        for (int i = 0; i < 10; ++i) {
            __delay_cycles(600000);
            P4OUT ^= 1 << 6;
        }
    } else {
        // Flash green LED if clock is good
        P1OUT = 0;
        for (int i = 0; i < 10; ++i) {
            __delay_cycles(600000);
            P1OUT ^= 1;
        }
    }
}

void vApplicationIdleHook( void ) {
    P1OUT = 0;
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
}

void vApplicationTickHook( void ) {
}

/******************************************************************************\
 *  task_blink_led implementation                                             *
\******************************************************************************/
StaticTask_t PERSISTENT blink_task;
StackType_t PERSISTENT blink_task_stack[configMINIMAL_STACK_SIZE];

void task_blink_led_start() {

    blink_led_task = xTaskCreateStatic(
        task_blink_led,
        "blink_led",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        blink_task_stack,
        &blink_task
    );
}

void task_blink_led(void * params) {
    volatile unsigned int sentinal = 0xBEEF;
    // taskENTER_CRITICAL();
    // uart_write_string(&standard_output, "Starting blink task\n");
    // taskEXIT_CRITICAL();
    for (;;) {
        // uart_write_string(&standard_output, "T1\n");
        // P1OUT++;
        P1OUT ^= 0x1;
        vTaskDelay(1);
    }
}

/******************************************************************************\
 *  task_pinav implementation                                 *
\******************************************************************************/
StaticTask_t PERSISTENT pinav_task;
StackType_t PERSISTENT pinav_task_stack[configMINIMAL_STACK_SIZE];
void task_pinav_start() {

    transmit_blink_signal_task = xTaskCreateStatic(
        task_pinav,
        "pinav",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        pinav_task_stack,
        &pinav_task
    );
}

void task_pinav(void * params) {
    taskENTER_CRITICAL();
    uart_write_string(&standard_output, "Starting pinav processing\n");
    taskEXIT_CRITICAL();
    for(;;) {
        P4OUT ^= 1 << 6;
        // Receive and parse a message if available
		if (uxQueueMessagesWaiting(pinav_sentence_queue_handle) > 0){
			xQueueReceive(pinav_sentence_queue_handle, pinav_parse_buffer, (TickType_t) 0); // TODO: handle failures on queue operations
			pinav_parser_status_t parse_result = parse_pinav_sentence(&pinav_parsed_sentence, pinav_parse_buffer);
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
	// enqueue complete sentence when \n is received or read buffer is filled
	if (chr == '\n' || pinav_read_buffer_index >= PINAV_MAX_SENTENCE_LEN){
		if(xQueueSendToBack(pinav_sentence_queue_handle, pinav_read_buffer, (TickType_t) 10) != pdTRUE){
			uart_write_string(&standard_output, "Pinav message enqueue failed");
		}
		pinav_read_buffer_index = 0;
	} else {	// otherwise add char to read buffer
		pinav_read_buffer[pinav_read_buffer_index] = chr;
		pinav_read_buffer_index++;
	}
}

/******************************************************************************\
 *  Random support functions and variables                                    *
 *      All shamelesly stolen from the demos in the FreeRTOS distribution.    *
\******************************************************************************/

/* Used for maintaining a 32-bit run time stats counter from a 16-bit timer. */
volatile uint32_t ulRunTimeCounterOverflows = 0;

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void ) {
    const unsigned short usACLK_Frequency_Hz = 32768;

    /* Ensure the timer is stopped. */
    TA0CTL = 0;

    /* Run the timer from the ACLK. */
    TA0CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA0CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

    /* Enable the interrupts. */
    TA0CCTL0 = CCIE;

    /* Start up clean. */
    TA0CTL |= TACLR;

    /* Up mode. */
    TA0CTL |= MC_1;
}


void vConfigureTimerForRunTimeStats( void ) {
    /* Configure a timer that is used as the time base for run time stats.  See
    http://www.freertos.org/rtos-run-time-stats.html */

    /* Ensure the timer is stopped. */
    TA1CTL = 0;

    /* Start up clean. */
    TA1CTL |= TACLR;

    /* Run the timer from the ACLK/8, continuous mode, interrupt enable. */
    TA1CTL = TASSEL_1 | ID__8 | MC__CONTINUOUS | TAIE;
}
__attribute__((interrupt(TIMER1_A1_VECTOR)))
void run_time_stats_isr( void ) {
    __bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
    TA1CTL &= ~TAIFG;
    /* 16-bit overflow, so add 17th bit. */
    ulRunTimeCounterOverflows += 0x10000;
}


/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
StaticTask_t PERSISTENT xIdleTaskTCB;
StackType_t PERSISTENT uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
StaticTask_t PERSISTENT xTimerTaskTCB;
StackType_t PERSISTENT uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
