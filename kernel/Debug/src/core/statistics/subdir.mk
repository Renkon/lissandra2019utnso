################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/statistics/metrics.c \
../src/core/statistics/stats.c 

OBJS += \
./src/core/statistics/metrics.o \
./src/core/statistics/stats.o 

C_DEPS += \
./src/core/statistics/metrics.d \
./src/core/statistics/stats.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/statistics/%.o: ../src/core/statistics/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Repositorios/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


