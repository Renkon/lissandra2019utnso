################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/core/utils/subdir.mk
-include src/core/statistics/subdir.mk
-include src/core/scheduler/subdir.mk
-include src/core/memories/subdir.mk
-include src/core/gossiping/subdir.mk
-include src/core/describer/subdir.mk
-include src/core/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: kernel

dependents:
	-cd /home/utnso/Repositorios/tp-2019-1c-Papito-code-en-Assembler/lib/Debug && $(MAKE) all

# Tool invocations
kernel: $(OBJS) $(USER_OBJS) /home/utnso/Repositorios/tp-2019-1c-Papito-code-en-Assembler/lib/Debug/liblib.a
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/Repositorios/tp-2019-1c-Papito-code-en-Assembler/lib/Debug" -o "kernel" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) kernel
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/Repositorios/tp-2019-1c-Papito-code-en-Assembler/lib/Debug/liblib.a:

-include ../makefile.targets
