#include <msp430.h>
#include <driverlib.h>


/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "uart.h"

/******************************************************************************\
 *  Static variables                                                          *
\******************************************************************************/

/// Standard UART output
static uart_t standard_output;

/// LED flash queue
static QueueHandle_t blink_queue = NULL;

const char * output_str = "hello, world!\r\n";
const char * got_data = "got data\r\n";

/******************************************************************************\
 *  Private functions                                                         *
\******************************************************************************/
/// Configures I/O pins
static void hardware_config();
/// Flasshes LEDs if the ACLK is configured at the expected frequency
static void test_aclk();

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

void task_blink_led(void * params);
void task_transmit_blink_signal(void * params);

/******************************************************************************\
 *  Function implementations                                                  *
\******************************************************************************/
int main(void) {

    hardware_config();

    P4OUT |= 0xFF;
    P1OUT |= 0xFF;

    test_aclk();

    uart_open(EUSCI_A0, BAUD_9600, &standard_output);
    blink_queue = xQueueCreate(4, sizeof(uint32_t));

    for(;;) {
        __delay_cycles(8000000UL);

        P1OUT ^= 0x01;
        uart_write_bytes(&standard_output, output_str, 15);
        P1OUT ^= 0x01;
    }

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

void vApplicationMallocFailedHook( void ) {
}

void vApplicationIdleHook( void ) {
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
}

void vApplicationTickHook( void ) {
    P4OUT ^= 1 << 6;
}


/******************************************************************************\
 *  Random support functions and variables                                    *
 *      All shamelesly stolen from the MSP430FR5969 demo in the FreeRTOS      *
 *      distribution.                                                         *
\******************************************************************************/

/* Used for maintaining a 32-bit run time stats counter from a 16-bit timer. */
volatile uint32_t ulRunTimeCounterOverflows = 0;

uint8_t __attribute__((section(".persistent"))) ucHeap[ configTOTAL_HEAP_SIZE ];

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
