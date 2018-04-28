################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS_Source/portable/MemMang/heap_4.obj: C:/Users/Nick/OneDrive/Virginia\ Tech/USIP/FreeRTOSv10.0.1/FreeRTOS/Source/portable/MemMang/heap_4.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Nick/OneDrive/Virginia Tech/USIP/FreeRTOSv10.0.1/FreeRTOS/Demo/MSP430X_MSP430FR5969_LaunchPad_IAR_CCS" --include_path="C:/Users/Nick/OneDrive/Virginia Tech/USIP/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/Users/Nick/OneDrive/Virginia Tech/USIP/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="C:/Users/Nick/OneDrive/Virginia Tech/USIP/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI" --include_path="C:/Users/Nick/OneDrive/Virginia Tech/USIP/FreeRTOSv10.0.1/FreeRTOS/Demo/Common/include" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --include_path="C:/Users/Nick/OneDrive/Virginia Tech/USIP/FreeRTOSv10.0.1/FreeRTOS/Demo/MSP430X_MSP430FR5969_LaunchPad_IAR_CCS/driverlib/MSP430FR5xx_6xx" -g --define=__MSP430FR5969__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="FreeRTOS_Source/portable/MemMang/heap_4.d" --obj_directory="FreeRTOS_Source/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


