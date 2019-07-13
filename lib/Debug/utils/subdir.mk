################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utils/binary_files.c \
../utils/dates.c \
../utils/file.c \
../utils/int_arrays.c \
../utils/network.c \
../utils/numbers.c \
../utils/operation_types.c \
../utils/partitions.c \
../utils/response.c \
../utils/serializer.c \
../utils/string.c 

OBJS += \
./utils/binary_files.o \
./utils/dates.o \
./utils/file.o \
./utils/int_arrays.o \
./utils/network.o \
./utils/numbers.o \
./utils/operation_types.o \
./utils/partitions.o \
./utils/response.o \
./utils/serializer.o \
./utils/string.o 

C_DEPS += \
./utils/binary_files.d \
./utils/dates.d \
./utils/file.d \
./utils/int_arrays.d \
./utils/network.d \
./utils/numbers.d \
./utils/operation_types.d \
./utils/partitions.d \
./utils/response.d \
./utils/serializer.d \
./utils/string.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Dfilesystem -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


