################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/config.c \
../src/memoria.c \
../src/memory_tables.c \
../src/memory_types.c \
../src/memory_utils.c 

OBJS += \
./src/config.o \
./src/memoria.o \
./src/memory_tables.o \
./src/memory_types.o \
./src/memory_utils.o 

C_DEPS += \
./src/config.d \
./src/memoria.d \
./src/memory_tables.d \
./src/memory_types.d \
./src/memory_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


