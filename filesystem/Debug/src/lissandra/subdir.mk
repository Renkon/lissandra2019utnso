################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lissandra/lissandra.c \
../src/lissandra/table_state_utils.c 

OBJS += \
./src/lissandra/lissandra.o \
./src/lissandra/table_state_utils.o 

C_DEPS += \
./src/lissandra/lissandra.d \
./src/lissandra/table_state_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/lissandra/%.o: ../src/lissandra/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


