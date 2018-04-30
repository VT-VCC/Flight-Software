################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
hal/gpio_native.obj: ../hal/gpio_native.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/opt/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal/hal_eusci" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="hal/gpio_native.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

hal/i2c.obj: ../hal/i2c.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/opt/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal/hal_eusci" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="hal/i2c.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

hal/imu.obj: ../hal/imu.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/opt/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal/hal_eusci" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="hal/imu.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

hal/mmc.obj: ../hal/mmc.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/opt/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal/hal_eusci" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="hal/mmc.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

hal/spi.obj: ../hal/spi.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/opt/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal/hal_eusci" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="hal/spi.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

hal/uart.obj: ../hal/uart.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/opt/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/opt/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal" --include_path="/home/souldia/Desktop/Sat/Flight-Software/CCS/Sensor_board_5969/hal/hal_eusci" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="hal/uart.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

