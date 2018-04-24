################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Full_Demo/Sample-CLI-commands.obj: /home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_CLI_Demos/Sample-CLI-commands.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/home/souldia/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Demo/Common/include" --include_path="/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" -g --define=__MSP430FR5969__ --display_error_number --diag_warning=225 --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="Full_Demo/Sample-CLI-commands.d_raw" --obj_directory="Full_Demo" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

Full_Demo/UARTCommandConsole.obj: /home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_CLI_Demos/UARTCommandConsole.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/home/souldia/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Demo/Common/include" --include_path="/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" -g --define=__MSP430FR5969__ --display_error_number --diag_warning=225 --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="Full_Demo/UARTCommandConsole.d_raw" --obj_directory="Full_Demo" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

Full_Demo/main_full.obj: ../Full_Demo/main_full.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/home/souldia/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Demo/Common/include" --include_path="/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" -g --define=__MSP430FR5969__ --display_error_number --diag_warning=225 --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="Full_Demo/main_full.d_raw" --obj_directory="Full_Demo" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

Full_Demo/serial.obj: ../Full_Demo/serial.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/home/souldia/ti/ccsv7/ccs_base/msp430/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI" --include_path="/home/souldia/Desktop/FreeRTOSv10.0.1/FreeRTOS/Demo/Common/include" --include_path="/home/souldia/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --include_path="/home/souldia/Desktop/satellite/Sensor_board_5969/driverlib/MSP430FR5xx_6xx" -g --define=__MSP430FR5969__ --display_error_number --diag_warning=225 --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="Full_Demo/serial.d_raw" --obj_directory="Full_Demo" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


