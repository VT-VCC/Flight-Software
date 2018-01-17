#include <msp430.h>
#include <driverlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "uart.h"
#include "spi.h"
#include "rfm.h"

#define PERSISTENT __attribute__((section(".persistent")))

/******************************************************************************\
 *  Static variables                                                          *
\******************************************************************************/

/// Standard UART output
static uart_t standard_output;

static spi_t spi_output;

/******************************************************************************\
 *  Private functions                                                         *
\******************************************************************************/
/// Configures I/O pins
static void hardware_config();

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

static TaskHandle_t PERSISTENT rfm_task;
void task_rfm_start();
void task_rfm(void * params);

/******************************************************************************\
 *  Function implementations                                                  *
\******************************************************************************/
int main(void) {
    hardware_config();

    uart_open(EUSCI_A0, BAUD_9600, &standard_output);

    task_rfm_start();

    uart_write_string(&standard_output, "Tasks initialized, starting scheduler\n");

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
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7|GPIO_PIN8|GPIO_PIN9|GPIO_PIN10|GPIO_PIN11|GPIO_PIN12|GPIO_PIN13|GPIO_PIN14|GPIO_PIN15);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7|GPIO_PIN8|GPIO_PIN9|GPIO_PIN10|GPIO_PIN11|GPIO_PIN12|GPIO_PIN13|GPIO_PIN14|GPIO_PIN15);
    
    // Configure UCA0TXD, UCA0RXD for UART over eUSCI_A0
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN1, GPIO_SECONDARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0, GPIO_SECONDARY_MODULE_FUNCTION);

    // Configure UCA3SIMO, UCA3SOMI for SPI over eUSCI_A3
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    // Use P3.0 as the RFM69's NSS (chip select)
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN0);

    // Use P1.5 as the RFM69's DIO0/IRQ interrupt
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5);

    // Configure GPIO to use LFXT
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ, GPIO_PIN4|GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
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

void vApplicationIdleHook( void ) {
    P1OUT = 0;
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
}

void vApplicationTickHook( void ) {
}

/******************************************************************************\
 *  task_rfm implementation                                 *
\******************************************************************************/

StaticTask_t PERSISTENT rfm_task_buffer;
StackType_t PERSISTENT rfm_task_stack[200];

void task_rfm_start() {
    rfm_task = xTaskCreateStatic(
        task_rfm,
        "rfm",
        sizeof(rfm_task_stack)/sizeof(rfm_task_stack[0]),
        NULL,
        2,
        rfm_task_stack,
        &rfm_task_buffer
    );
}

void dump_rfm_regs(rfm_t * radio);

void task_rfm(void * params) {
    taskENTER_CRITICAL();
    uart_write_string(&standard_output, "Starting RFM task\n");
    taskEXIT_CRITICAL();

    spi_open(EUSCI_A3, 32768/4, &spi_output);

    rfm_t radio;
    if (rfm_open(&radio, &spi_output, &P3OUT, 1)) {
        uart_write_string(&standard_output, "RFM initialized\n");
    }
    else {
        uart_write_string(&standard_output, "RFM failed to initialize\n");
    }

    // Set frequency to 433 MHz
    if (rfm_set_frequency(&radio, 433000000) != RFM_NO_ERROR) {
        uart_write_string(&standard_output, "[ERROR] rfm_set_frequency\n");
        return;
    }

    // Put a packet into the buffer
    uint8_t msg[6] = { 'H', 'e', 'l', 'l', 'o', '\0' };
    if (rfm_write_fifo(&radio, msg, 6) != RFM_NO_ERROR) {
        uart_write_string(&standard_output, "[ERROR] rfm_write_fifo\n");
        return;
    }

    if (rfm_set_mode(&radio, RFM_MODE_TX) != RFM_NO_ERROR) {
        uart_write_string(&standard_output, "[ERROR] rfm_set_mode\n");
        return;
    }

    //dump_rfm_regs(&radio);
}

void dump_rfm_regs(rfm_t * radio) {
    uart_write_string(&standard_output, "==============================\nDumping RFM Registers\n==============================\n");

    char buffer[100];
    int len;

    int capVal;
    uint8_t modeFSK = 0;
    int bitRate = 0;
    int freqDev = 0;
    long freqCenter = 0;

    for (uint8_t addr = 1; addr <= 0x4f; ++addr) {
        uint8_t value;
        if (rfm_read_reg(radio, addr, &value) != RFM_NO_ERROR) {
            uart_write_string(&standard_output, "Failed to read RFM address\n");
            continue;
        }

        len = snprintf(buffer, 100, "%02x: %02x\n", addr, value);
        uart_write_bytes(&standard_output, buffer, len);

        switch ( addr ) 
        {
            case 0x1 : {
                uart_write_string(&standard_output, "Controls the automatic Sequencer ( see section 4.2 )\nSequencerOff : " );
                if ( 0x80 & value ) {
                    uart_write_string(&standard_output, "1 -> Mode is forced by the user\n" );
                } else {
                    uart_write_string(&standard_output, "0 -> Operating mode as selected with Mode bits in RegOpMode is automatically reached with the Sequencer\n" );
                }
                
                uart_write_string(&standard_output, "\nEnables Listen mode, should be enabled whilst in Standby mode:\nListenOn : " );
                if ( 0x40 & value ) {
                    uart_write_string(&standard_output, "1 -> On\n" );
                } else {
                    uart_write_string(&standard_output, "0 -> Off ( see section 4.3)\n" );
                }
                
                uart_write_string(&standard_output, "\nAborts Listen mode when set together with ListenOn=0 See section 4.3.4 for details (Always reads 0.)\n" );
                if ( 0x20 & value ) {
                    uart_write_string(&standard_output, "ERROR - ListenAbort should NEVER return 1 this is a write only register\n" );
                }
                
                uart_write_string(&standard_output,"\nTransceiver's operating modes:\nMode : ");
                capVal = (value >> 2) & 0x7;
                if ( capVal == 0b000 ) {
                    uart_write_string(&standard_output, "000 -> Sleep mode (SLEEP)\n" );
                } else if ( capVal = 0b001 ) {
                    uart_write_string(&standard_output, "001 -> Standby mode (STDBY)\n" );
                } else if ( capVal = 0b010 ) {
                    uart_write_string(&standard_output, "010 -> Frequency Synthesizer mode (FS)\n" );
                } else if ( capVal = 0b011 ) {
                    uart_write_string(&standard_output, "011 -> Transmitter mode (TX)\n" );
                } else if ( capVal = 0b100 ) {
                    uart_write_string(&standard_output, "100 -> Receiver Mode (RX)\n" );
                } else {
                    len = snprintf(buffer, 100, "%02x", capVal);
                    uart_write_bytes(&standard_output, buffer, len);
                    uart_write_string(&standard_output, " -> RESERVED\n" );
                }
                uart_write_string(&standard_output, "\n" );
                break;
            }
            
            case 0x2 : {
            
                uart_write_string(&standard_output,"Data Processing mode:\nDataMode : ");
                capVal = (value >> 5) & 0x3;
                if ( capVal == 0b00 ) {
                    uart_write_string(&standard_output, "00 -> Packet mode\n" );
                } else if ( capVal == 0b01 ) {
                    uart_write_string(&standard_output, "01 -> reserved\n" );
                } else if ( capVal == 0b10 ) {
                    uart_write_string(&standard_output, "10 -> Continuous mode with bit synchronizer\n" );
                } else if ( capVal == 0b11 ) {
                    uart_write_string(&standard_output, "11 -> Continuous mode without bit synchronizer\n" );
                }
                
                uart_write_string(&standard_output,"\nModulation scheme:\nModulation Type : ");
                capVal = (value >> 3) & 0x3;
                if ( capVal == 0b00 ) {
                    uart_write_string(&standard_output, "00 -> FSK\n" );
                    modeFSK = 1;
                } else if ( capVal == 0b01 ) {
                    uart_write_string(&standard_output, "01 -> OOK\n" );
                } else if ( capVal == 0b10 ) {
                    uart_write_string(&standard_output, "10 -> reserved\n" );
                } else if ( capVal == 0b11 ) {
                    uart_write_string(&standard_output, "11 -> reserved\n" );
                }
                
                uart_write_string(&standard_output,"\nData shaping: ");
                if ( modeFSK ) {
                    uart_write_string(&standard_output, "in FSK:\n" );
                } else {
                    uart_write_string(&standard_output, "in OOK:\n" );
                }
                uart_write_string(&standard_output,"ModulationShaping : ");
                capVal = value & 0x3;
                if ( modeFSK ) {
                    if ( capVal == 0b00 ) {
                        uart_write_string(&standard_output, "00 -> no shaping\n" );
                    } else if ( capVal == 0b01 ) {
                        uart_write_string(&standard_output, "01 -> Gaussian filter, BT = 1.0\n" );
                    } else if ( capVal == 0b10 ) {
                        uart_write_string(&standard_output, "10 -> Gaussian filter, BT = 0.5\n" );
                    } else if ( capVal == 0b11 ) {
                        uart_write_string(&standard_output, "11 -> Gaussian filter, BT = 0.3\n" );
                    }
                } else {
                    if ( capVal == 0b00 ) {
                        uart_write_string(&standard_output, "00 -> no shaping\n" );
                    } else if ( capVal == 0b01 ) {
                        uart_write_string(&standard_output, "01 -> filtering with f(cutoff) = BR\n" );
                    } else if ( capVal == 0b10 ) {
                        uart_write_string(&standard_output, "10 -> filtering with f(cutoff) = 2*BR\n" );
                    } else if ( capVal == 0b11 ) {
                        uart_write_string(&standard_output, "ERROR - 11 is reserved\n" );
                    }
                }
                
                uart_write_string(&standard_output, "\n" );
                break;
            }
            
            case 0x3 : {
                bitRate = (value << 8);
                break;
            }
            
            case 0x4 : {
                bitRate |= value;
                uart_write_string(&standard_output, "Bit Rate (Chip Rate when Manchester encoding is enabled)\nBitRate : ");
                unsigned long val = 32UL * 1000UL * 1000UL / bitRate;
                len = snprintf(buffer, 100, "%d\n", val);
                uart_write_bytes(&standard_output, buffer, len);
                break;
            }
            
            case 0x5 : {
                freqDev = ( (value & 0x3f) << 8 );
                break;
            }
            
            case 0x6 : {
                freqDev |= value;
                uart_write_string(&standard_output, "Frequency deviation\nFdev : " );
                unsigned long val = 61UL * freqDev;
                len = snprintf(buffer, 100, "%d\n", val);
                uart_write_bytes(&standard_output, buffer, len);
                break;
            }
            
            case 0x7 : {
                unsigned long tempVal = value;
                freqCenter = ( tempVal << 16 );
                break;
            }
           
            case 0x8 : {
                unsigned long tempVal = value;
                freqCenter = freqCenter | ( tempVal << 8 );
                break;
            }

            case 0x9 : {        
                freqCenter = freqCenter | value;
                uart_write_string(&standard_output, "RF Carrier frequency\nFRF : " );
                unsigned long val = 61UL * freqCenter;
                len = snprintf(buffer, 100, "%d\n", val);
                uart_write_bytes(&standard_output, buffer, len);
                break;
            }

            case 0xa : {
                uart_write_string(&standard_output, "RC calibration control & status\nRcCalDone : " );
                if ( 0x40 & value ) {
                    uart_write_string(&standard_output, "1 -> RC calibration is over\n" );
                } else {
                    uart_write_string(&standard_output, "0 -> RC calibration is in progress\n" );
                }
            
                uart_write_string(&standard_output, "\n" );
                break;
            }

            case 0xb : {
                uart_write_string(&standard_output, "Improved AFC routine for signals with modulation index lower than 2.  Refer to section 3.4.16 for details\nAfcLowBetaOn : " );
                if ( 0x20 & value ) {
                    uart_write_string(&standard_output, "1 -> Improved AFC routine\n" );
                } else {
                    uart_write_string(&standard_output, "0 -> Standard AFC routine\n" );
                }
                uart_write_string(&standard_output, "\n" );
                break;
            }
            
            case 0xc : {
                uart_write_string(&standard_output, "Reserved\n\n" );
                break;
            }

            case 0xd : {
                uint8_t val;
                uart_write_string(&standard_output, "Resolution of Listen mode Idle time (calibrated RC osc):\nListenResolIdle : " );
                val = value >> 6;
                if ( val == 0b00 ) {
                    uart_write_string(&standard_output, "00 -> reserved\n" );
                } else if ( val == 0b01 ) {
                    uart_write_string(&standard_output, "01 -> 64 us\n" );
                } else if ( val == 0b10 ) {
                    uart_write_string(&standard_output, "10 -> 4.1 ms\n" );
                } else if ( val == 0b11 ) {
                    uart_write_string(&standard_output, "11 -> 262 ms\n" );
                }
                
                uart_write_string(&standard_output, "\nResolution of Listen mode Rx time (calibrated RC osc):\nListenResolRx : " );
                val = (value >> 4) & 0x3;
                if ( val == 0b00 ) {
                    uart_write_string(&standard_output, "00 -> reserved\n" );
                } else if ( val == 0b01 ) {
                    uart_write_string(&standard_output, "01 -> 64 us\n" );
                } else if ( val == 0b10 ) {
                    uart_write_string(&standard_output, "10 -> 4.1 ms\n" );
                } else if ( val == 0b11 ) {
                    uart_write_string(&standard_output, "11 -> 262 ms\n" );
                }

                uart_write_string(&standard_output, "\nCriteria for packet acceptance in Listen mode:\nListenCriteria : " );
                if ( 0x8 & value ) {
                    uart_write_string(&standard_output, "1 -> signal strength is above RssiThreshold and SyncAddress matched\n" );
                } else {
                    uart_write_string(&standard_output, "0 -> signal strength is above RssiThreshold\n" );
                }
                
                uart_write_string(&standard_output, "\nAction taken after acceptance of a packet in Listen mode:\nListenEnd : " );
                val = (value >> 1 ) & 0x3;
                if ( val == 0b00 ) {
                    uart_write_string(&standard_output, "00 -> chip stays in Rx mode. Listen mode stops and must be disabled (see section 4.3)\n" );
                } else if ( val == 0b01 ) {
                    uart_write_string(&standard_output, "01 -> chip stays in Rx mode until PayloadReady or Timeout interrupt occurs.  It then goes to the mode defined by Mode. Listen mode stops and must be disabled (see section 4.3)\n" );
                } else if ( val == 0b10 ) {
                    uart_write_string(&standard_output, "10 -> chip stays in Rx mode until PayloadReady or Timeout occurs.  Listen mode then resumes in Idle state.  FIFO content is lost at next Rx wakeup.\n" );
                } else if ( val == 0b11 ) {
                    uart_write_string(&standard_output, "11 -> Reserved\n" );
                }
                
                
                uart_write_string(&standard_output, "\n" );
                break;
            }
            
            default : {
            }
        }
    }

    uart_write_string(&standard_output, "==============================\nEnd of RFM Dump\n==============================\n");
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
