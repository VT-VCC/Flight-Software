#include <msp430.h>
#include <driverlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "gpio.h"
#include "gpio_native.h"

#include "uart.h"
#include "spi.h"
#include "i2c.h"

#include "imu.h"
#include "mmc.h"

#define PERSISTENT __attribute__((section(".persistent")))

/******************************************************************************\
 *  Static variables                                                          *
\******************************************************************************/

/// Standard UART output
static uart_t standard_output;

/// IMU/RFM SPI output
static spi_t spi_output;

// Print a formatted message across the UART output
#define DEBUG_VA_ARGS(...) , ## __VA_ARGS__
#define DEBUG(format, ...) do { \
        char buffer[255]; \
        int len = snprintf(buffer, 255, (format) DEBUG_VA_ARGS(__VA_ARGS__)); \
        uart_write_bytes(&standard_output, buffer, len); \
    } while(0)
#define WTF() DEBUG("[ERROR] %s (%s:%d)\n", __func__, __FILE__, __LINE__)

/******************************************************************************\
 *  Private functions                                                         *
\******************************************************************************/
/// Configures I/O pins
static void hardware_config(void);
static void spi_hardware_config(void);
static void i2c_hardware_config(void);

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

static TaskHandle_t PERSISTENT i2c_task;
void task_i2c_start();
void task_i2c(void * params);

static TaskHandle_t PERSISTENT imu_task;
void task_imu_start();
void task_imu(void * params);

static TaskHandle_t PERSISTENT mmc_task;
void task_mmc_start();
void task_mmc(void * params);

/******************************************************************************\
 *  Function implementations                                                  *
\******************************************************************************/
int main(void) {
    hardware_config();

    uart_open(EUSCI_A0, BAUD_9600, &standard_output);

    task_mmc_start();

    uart_write_string(&standard_output, "Tasks initialized, starting scheduler\n");

    vTaskStartScheduler();

    // there is no way to get here since we are using statically allocated
    // kernel structures

    return 0;
}

static void hardware_config(void) {
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

static void spi_hardware_config(void) {
    // Configure UCA3SIMO, UCA3SOMI for SPI over eUSCI_A3
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);

    // Use P3.0 as the RFM69's NSS (chip select)
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN0);

    // Use P as the RFM69's RST (reset)
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN0);

    // Use P1.5 as the RFM69's DIO0/IRQ interrupt
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5);
}

static void i2c_hardware_config(void) {
    // Configure UCB2SDA, UCB2SCL for I2C over eUSCI_B2
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
}

void vApplicationIdleHook( void ) { }

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) { }

void vApplicationTickHook( void ) { }

/******************************************************************************\
 *  task_i2c implementation                                 *
\******************************************************************************/

#define I2C_TASK_STACK_DEPTH 200

StaticTask_t PERSISTENT i2c_task_buffer;
StackType_t PERSISTENT i2c_task_stack[I2C_TASK_STACK_DEPTH];

void task_i2c_start() {
    i2c_task = xTaskCreateStatic(
        task_i2c,
        "i2c",
        I2C_TASK_STACK_DEPTH,
        NULL,
        2,
        i2c_task_stack,
        &i2c_task_buffer
    );
}

void task_i2c(void * params) {
    taskENTER_CRITICAL();
    i2c_hardware_config();
    DEBUG("Starting I2C task\n");
    taskEXIT_CRITICAL();

    i2c_t device;
    if (!i2c_open(EUSCI_B2_BASE, I2C_DATA_RATE_400KBPS, &device)) {
        WTF();
        return;
    }

    DEBUG("I2C device initialized\n");

    i2c_error_t err;

    // Get MPU WHO_AM_I
    {
        DEBUG("a\n");
        const uint8_t mpu_address = 0x69;
        const uint8_t mpu_who_am_i = 0x75;
        err = i2c_write_byte(&device, mpu_address, mpu_who_am_i, I2C_NO_STOP);
        if (err != I2C_NO_ERROR) {
            WTF();
        }

        DEBUG("b\n");

        uint8_t recv;
        err = i2c_read_byte(&device, mpu_address, &recv);
        if (err != I2C_NO_ERROR) {
            WTF();
        }

        DEBUG("c\n");
    }


    DEBUG("I2C task done\n");

    DEBUG("%d\n", uxTaskGetStackHighWaterMark(NULL));

    // Check that we haven't overflowed the stack
    int remaining_frames = I2C_TASK_STACK_DEPTH - uxTaskGetStackHighWaterMark(NULL);
    if (remaining_frames < 20) {
        DEBUG("!!! %d frames left on the stack !!!\n", remaining_frames);
    }
}


/******************************************************************************\
 *  task_imu implementation                                 *
\******************************************************************************/

#define IMU_TASK_STACK_DEPTH 200

StaticTask_t PERSISTENT imu_task_buffer;
StackType_t PERSISTENT imu_task_stack[IMU_TASK_STACK_DEPTH];

void task_imu_start() {
    imu_task = xTaskCreateStatic(
        task_imu,
        "imu",
        IMU_TASK_STACK_DEPTH,
        NULL,
        2,
        imu_task_stack,
        &imu_task_buffer
    );
}

void task_imu(void * params) {
    taskENTER_CRITICAL();
    spi_hardware_config();
    DEBUG("Starting IMU task\n");
    taskEXIT_CRITICAL();

    spi_open(EUSCI_A3, 32768/4, &spi_output);

    imu_t device;
    if (imu_open(&device, &spi_output, &P3OUT, 1)) {
        DEBUG("IMU initialized\n");
    }
    else {
        DEBUG("IMU failed to initialize\n");
    }

    // Check that we haven't overflowed the stack
    int remaining_frames = IMU_TASK_STACK_DEPTH - uxTaskGetStackHighWaterMark(NULL);
    if (remaining_frames < 20) {
        DEBUG("!!! %d frames left on the stack !!!\n", remaining_frames);
    }
}


/******************************************************************************\
 *  task_mmc implementation                                 *
\******************************************************************************/

#define MMC_TASK_STACK_DEPTH 200

StaticTask_t PERSISTENT mmc_task_buffer;
StackType_t PERSISTENT mmc_task_stack[MMC_TASK_STACK_DEPTH];

void task_mmc_start() {
    mmc_task = xTaskCreateStatic(
        task_mmc,
        "mmc",
        MMC_TASK_STACK_DEPTH,
        NULL,
        2,
        mmc_task_stack,
        &mmc_task_buffer
    );
}

void task_mmc(void * params) {
    taskENTER_CRITICAL();
    spi_hardware_config();
    DEBUG("Starting MMC task\n");
    taskEXIT_CRITICAL();

    spi_open(EUSCI_A3, 32768/4, &spi_output);

    mmc_t device;
    if (mmc_init(&device, &spi_output, GPIO_PORT_3, GPIO_PIN_1)) {
        DEBUG("MMC initialized\n");
    }
    else {
        DEBUG("MMC failed to initialize\n");
    }

    // Check that we haven't overflowed the stack
    int remaining_frames = MMC_TASK_STACK_DEPTH - uxTaskGetStackHighWaterMark(NULL);
    if (remaining_frames < 20) {
        DEBUG("!!! %d frames left on the stack !!!\n", remaining_frames);
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
