################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../generic_client.c \
../generic_config.c \
../generic_console.c \
../generic_logger.c \
../generic_server.c 

OBJS += \
./generic_client.o \
./generic_config.o \
./generic_console.o \
./generic_logger.o \
./generic_server.o 

C_DEPS += \
./generic_client.d \
./generic_config.d \
./generic_console.d \
./generic_logger.d \
./generic_server.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dfilesystem -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


