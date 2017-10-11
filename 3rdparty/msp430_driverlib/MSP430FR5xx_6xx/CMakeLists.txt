# Builds the MSP430 support library
add_library(msp430_driverlib STATIC
  adc12_b.c
  aes256.c
  comp_e.c
  crc32.c
  crc.c
  cs.c
  dma.c
  esi.c
  eusci_a_spi.c
  eusci_a_uart.c
  eusci_b_i2c.c
  eusci_b_spi.c
  framctl_a.c
  framctl.c
  gpio.c
  lcd_c.c
  mpu.c
  mpy32.c
  pmm.c
  ram.c
  ref_a.c
  rtc_b.c
  rtc_c.c
  sfr.c
  sysctl.c
  timer_a.c
  timer_b.c
  tlv.c
  wdt_a.c
)

# Disable int to pointer casts since these should always be safe inside the
# driver library
target_compile_options(msp430_driverlib PRIVATE -Wno-int-to-pointer-cast)
