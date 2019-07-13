################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/memories/memories.c 

OBJS += \
./src/core/memories/memories.o 

C_DEPS += \
./src/core/memories/memories.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/memories/%.o: ../src/core/memories/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


