################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/aht20.c \
../Src/ldr.c \
../Src/main.c \
../Src/nrfl2401.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/uart.c 

OBJS += \
./Src/aht20.o \
./Src/ldr.o \
./Src/main.o \
./Src/nrfl2401.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/uart.o 

C_DEPS += \
./Src/aht20.d \
./Src/ldr.d \
./Src/main.d \
./Src/nrfl2401.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DSTM32F103xB=STM32F103xB -c -I"/home/ariane/Documentos/faculdade/semestre_5/microcontroladores/projeto_final/Inc" -I"/home/ariane/Documentos/faculdade/semestre_5/microcontroladores/projeto_final/F1_Header/Include" -I"/home/ariane/Documentos/faculdade/semestre_5/microcontroladores/projeto_final/F1_Header/Device/ST/STM32F1xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/aht20.cyclo ./Src/aht20.d ./Src/aht20.o ./Src/aht20.su ./Src/ldr.cyclo ./Src/ldr.d ./Src/ldr.o ./Src/ldr.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/nrfl2401.cyclo ./Src/nrfl2401.d ./Src/nrfl2401.o ./Src/nrfl2401.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su

.PHONY: clean-Src

