################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/scheduler/long_term_scheduler.c \
../src/core/scheduler/scheduler.c \
../src/core/scheduler/short_term_scheduler.c 

OBJS += \
./src/core/scheduler/long_term_scheduler.o \
./src/core/scheduler/scheduler.o \
./src/core/scheduler/short_term_scheduler.o 

C_DEPS += \
./src/core/scheduler/long_term_scheduler.d \
./src/core/scheduler/scheduler.d \
./src/core/scheduler/short_term_scheduler.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/scheduler/%.o: ../src/core/scheduler/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


