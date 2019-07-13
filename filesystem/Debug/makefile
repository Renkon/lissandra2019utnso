################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/lissandra/subdir.mk
-include src/filesystem/subdir.mk
-include src/core/subdir.mk
-include src/compactor/subdir.mk
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
all: filesystem

dependents:
	-cd /home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib/Debug && $(MAKE) all

# Tool invocations
filesystem: $(OBJS) $(USER_OBJS) /home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib/Debug/liblib.a
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib/Debug" -o "filesystem" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) filesystem
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/lib/Debug/liblib.a:

-include ../makefile.targets
