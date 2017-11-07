#include "spi.h"

#include <assert.h>

static uint16_t BASE_ADDRESSES[EUSCI_count] = {
#ifdef EUSCI_A0_BASE
    EUSCI_A0_BASE,
#endif
#ifdef EUSCI_A1_BASE
    EUSCI_A1_BASE,
#endif
#ifdef EUSCI_A2_BASE
    EUSCI_A2_BASE,
#endif
#ifdef EUSCI_A3_BASE
    EUSCI_A3_BASE,
#endif
#ifdef EUSCI_B0_BASE
    EUSCI_B0_BASE,
#endif
#ifdef EUSCI_B1_BASE
    EUSCI_B1_BASE,
#endif
#ifdef EUSCI_B2_BASE
    EUSCI_B2_BASE,
#endif
#ifdef EUSCI_B3_BASE
    EUSCI_B3_BASE,
#endif
    
    volatile int read_byte = 0;
    
    __attribute__((interrupt(USCI_A0_VECTOR)))
    void USCI_A0_ISR(void) {
        switch (__even_in_range(UCA0IV, 18)) {
            case USCI_NONE: break;
            case USCI_SPI_UCRXIFG:
                P4OUT = 1 << 6;
                read_byte = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
                break;
            case USCI_SPI_UCTXIFG: break;
            case USCI_SPI_UCSTTIFG: break;
            case USCI_SPI_UCTXCPTIFG: break;
        }
    }
    __attribute__((interrupt(USCI_A1_VECTOR)))
    void USCI_A1_ISR(void) {
        switch (__even_in_range(UCA1IV, 18)) {
            case USCI_NONE: break;
            case USCI_SPI_UCRXIFG:
                read_byte = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
                break;
            case USCI_SPI_UCTXIFG: break;
            case USCI_SPI_UCSTTIFG: break;
            case USCI_SPI_UCTXCPTIFG: break;
        }
    }
    bool spi_open(eusci_t eusci, spi_t * out) {
        EUSCI_A_UART_initParam param = {0};
        uint16_t baseAddress = BASE_ADDRESSES[eusci];
        EUSCI_A_SPI_initMasterParam;
        param.msbFirst = EUSCI B SPI MSB FIRST;
        param.clockPhase = EUSCI B SPI PHASE DATA CHANGED ONFIRST CAPTURED ON NEXT; param.clockPolarity = EUSCI B SPI CLOCKPOLARITY INACTIVITY HIGH; param.spiMode = EUSCI B SPI 3PIN;
        EUSCI B SPI initSlave(EUSCI B0 BASE, &param);
    
    
    extern void EUSCI_B_SPI_clearInterrupt(uint16_t baseAddress,
                                           uint8_t mask);
                                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
                                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
    
    extern void EUSCI_B_SPI_enableInterrupt(uint16_t baseAddress,
                                            uint8_t mask);
                                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
                                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
    
    extern uint8 t EUSCI B SPI getInterruptStatus ( uint16 t baseAddress, uint8 t mask);
                                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
                                        EUSCI_B_SPI_TRANSMIT_INTERRUPT
    
    extern uint32_t EUSCI_B_SPI_getReceiveBufferAddress(uint16_t baseAddress);
                                        //return address of RX Buffer

        
}
    spi_error_t spi_write_byte(spi_t * channel, uint8_t byte) {
        // XXX: TODO: should acquire/check that the current thread has acquired a
        // lock on the EUSCI module
        EUSCI_A_SPI_transmitData(BASE_ADDRESSES[channel->eusci], byte);
            
        if (read_byte) {
            for (int i = 0; i < 32; ++i) {
                P4OUT ^= 1 << 6;
                __delay_cycles(800000UL);
            }
            read_byte = 0;
        }
            
        return SPI_NO_ERROR;
    }
        
};
