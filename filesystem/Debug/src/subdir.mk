################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/config.c \
../src/directorys.c \
../src/fs_lists.c \
../src/table_utils.c \
../src/tkvs_utils.c 

OBJS += \
./src/config.o \
./src/directorys.o \
./src/fs_lists.o \
./src/table_utils.o \
./src/tkvs_utils.o 

C_DEPS += \
./src/config.d \
./src/directorys.d \
./src/fs_lists.d \
./src/table_utils.d \
./src/tkvs_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


