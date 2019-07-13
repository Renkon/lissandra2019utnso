################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/filesystem/filesystem.c \
../src/filesystem/key_searching.c 

OBJS += \
./src/filesystem/filesystem.o \
./src/filesystem/key_searching.o 

C_DEPS += \
./src/filesystem/filesystem.d \
./src/filesystem/key_searching.d 


# Each subdirectory must supply rules for building sources it contributes
src/filesystem/%.o: ../src/filesystem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Repositorios/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


