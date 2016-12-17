################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Generated_Code/AdcLdd1.c" \
"../Generated_Code/BitIoLdd1.c" \
"../Generated_Code/BitIoLdd2.c" \
"../Generated_Code/BitIoLdd3.c" \
"../Generated_Code/BitIoLdd4.c" \
"../Generated_Code/BitsIoLdd1.c" \
"../Generated_Code/Button.c" \
"../Generated_Code/Conversor.c" \
"../Generated_Code/Cpu.c" \
"../Generated_Code/DataCmd.c" \
"../Generated_Code/Enable.c" \
"../Generated_Code/LED.c" \
"../Generated_Code/PE_LDD.c" \
"../Generated_Code/RS.c" \
"../Generated_Code/TU1.c" \
"../Generated_Code/TimerIntLdd1.c" \
"../Generated_Code/UTIL1.c" \
"../Generated_Code/Vectors.c" \
"../Generated_Code/timer.c" \

C_SRCS += \
../Generated_Code/AdcLdd1.c \
../Generated_Code/BitIoLdd1.c \
../Generated_Code/BitIoLdd2.c \
../Generated_Code/BitIoLdd3.c \
../Generated_Code/BitIoLdd4.c \
../Generated_Code/BitsIoLdd1.c \
../Generated_Code/Button.c \
../Generated_Code/Conversor.c \
../Generated_Code/Cpu.c \
../Generated_Code/DataCmd.c \
../Generated_Code/Enable.c \
../Generated_Code/LED.c \
../Generated_Code/PE_LDD.c \
../Generated_Code/RS.c \
../Generated_Code/TU1.c \
../Generated_Code/TimerIntLdd1.c \
../Generated_Code/UTIL1.c \
../Generated_Code/Vectors.c \
../Generated_Code/timer.c \

OBJS += \
./Generated_Code/AdcLdd1.o \
./Generated_Code/BitIoLdd1.o \
./Generated_Code/BitIoLdd2.o \
./Generated_Code/BitIoLdd3.o \
./Generated_Code/BitIoLdd4.o \
./Generated_Code/BitsIoLdd1.o \
./Generated_Code/Button.o \
./Generated_Code/Conversor.o \
./Generated_Code/Cpu.o \
./Generated_Code/DataCmd.o \
./Generated_Code/Enable.o \
./Generated_Code/LED.o \
./Generated_Code/PE_LDD.o \
./Generated_Code/RS.o \
./Generated_Code/TU1.o \
./Generated_Code/TimerIntLdd1.o \
./Generated_Code/UTIL1.o \
./Generated_Code/Vectors.o \
./Generated_Code/timer.o \

C_DEPS += \
./Generated_Code/AdcLdd1.d \
./Generated_Code/BitIoLdd1.d \
./Generated_Code/BitIoLdd2.d \
./Generated_Code/BitIoLdd3.d \
./Generated_Code/BitIoLdd4.d \
./Generated_Code/BitsIoLdd1.d \
./Generated_Code/Button.d \
./Generated_Code/Conversor.d \
./Generated_Code/Cpu.d \
./Generated_Code/DataCmd.d \
./Generated_Code/Enable.d \
./Generated_Code/LED.d \
./Generated_Code/PE_LDD.d \
./Generated_Code/RS.d \
./Generated_Code/TU1.d \
./Generated_Code/TimerIntLdd1.d \
./Generated_Code/UTIL1.d \
./Generated_Code/Vectors.d \
./Generated_Code/timer.d \

OBJS_QUOTED += \
"./Generated_Code/AdcLdd1.o" \
"./Generated_Code/BitIoLdd1.o" \
"./Generated_Code/BitIoLdd2.o" \
"./Generated_Code/BitIoLdd3.o" \
"./Generated_Code/BitIoLdd4.o" \
"./Generated_Code/BitsIoLdd1.o" \
"./Generated_Code/Button.o" \
"./Generated_Code/Conversor.o" \
"./Generated_Code/Cpu.o" \
"./Generated_Code/DataCmd.o" \
"./Generated_Code/Enable.o" \
"./Generated_Code/LED.o" \
"./Generated_Code/PE_LDD.o" \
"./Generated_Code/RS.o" \
"./Generated_Code/TU1.o" \
"./Generated_Code/TimerIntLdd1.o" \
"./Generated_Code/UTIL1.o" \
"./Generated_Code/Vectors.o" \
"./Generated_Code/timer.o" \

C_DEPS_QUOTED += \
"./Generated_Code/AdcLdd1.d" \
"./Generated_Code/BitIoLdd1.d" \
"./Generated_Code/BitIoLdd2.d" \
"./Generated_Code/BitIoLdd3.d" \
"./Generated_Code/BitIoLdd4.d" \
"./Generated_Code/BitsIoLdd1.d" \
"./Generated_Code/Button.d" \
"./Generated_Code/Conversor.d" \
"./Generated_Code/Cpu.d" \
"./Generated_Code/DataCmd.d" \
"./Generated_Code/Enable.d" \
"./Generated_Code/LED.d" \
"./Generated_Code/PE_LDD.d" \
"./Generated_Code/RS.d" \
"./Generated_Code/TU1.d" \
"./Generated_Code/TimerIntLdd1.d" \
"./Generated_Code/UTIL1.d" \
"./Generated_Code/Vectors.d" \
"./Generated_Code/timer.d" \

OBJS_OS_FORMAT += \
./Generated_Code/AdcLdd1.o \
./Generated_Code/BitIoLdd1.o \
./Generated_Code/BitIoLdd2.o \
./Generated_Code/BitIoLdd3.o \
./Generated_Code/BitIoLdd4.o \
./Generated_Code/BitsIoLdd1.o \
./Generated_Code/Button.o \
./Generated_Code/Conversor.o \
./Generated_Code/Cpu.o \
./Generated_Code/DataCmd.o \
./Generated_Code/Enable.o \
./Generated_Code/LED.o \
./Generated_Code/PE_LDD.o \
./Generated_Code/RS.o \
./Generated_Code/TU1.o \
./Generated_Code/TimerIntLdd1.o \
./Generated_Code/UTIL1.o \
./Generated_Code/Vectors.o \
./Generated_Code/timer.o \


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/AdcLdd1.o: ../Generated_Code/AdcLdd1.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/AdcLdd1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/AdcLdd1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/BitIoLdd1.o: ../Generated_Code/BitIoLdd1.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/BitIoLdd1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/BitIoLdd1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/BitIoLdd2.o: ../Generated_Code/BitIoLdd2.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/BitIoLdd2.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/BitIoLdd2.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/BitIoLdd3.o: ../Generated_Code/BitIoLdd3.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/BitIoLdd3.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/BitIoLdd3.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/BitIoLdd4.o: ../Generated_Code/BitIoLdd4.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/BitIoLdd4.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/BitIoLdd4.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/BitsIoLdd1.o: ../Generated_Code/BitsIoLdd1.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/BitsIoLdd1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/BitsIoLdd1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/Button.o: ../Generated_Code/Button.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Button.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Button.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/Conversor.o: ../Generated_Code/Conversor.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Conversor.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Conversor.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/Cpu.o: ../Generated_Code/Cpu.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Cpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Cpu.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/DataCmd.o: ../Generated_Code/DataCmd.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/DataCmd.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/DataCmd.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/Enable.o: ../Generated_Code/Enable.c
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Enable.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Enable.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/LED.o: ../Generated_Code/LED.c
	@echo 'Building file: $<'
	@echo 'Executing target #17 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/LED.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/LED.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/PE_LDD.o: ../Generated_Code/PE_LDD.c
	@echo 'Building file: $<'
	@echo 'Executing target #18 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/PE_LDD.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/PE_LDD.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/RS.o: ../Generated_Code/RS.c
	@echo 'Building file: $<'
	@echo 'Executing target #19 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/RS.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/RS.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/TU1.o: ../Generated_Code/TU1.c
	@echo 'Building file: $<'
	@echo 'Executing target #20 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/TU1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/TU1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/TimerIntLdd1.o: ../Generated_Code/TimerIntLdd1.c
	@echo 'Building file: $<'
	@echo 'Executing target #21 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/TimerIntLdd1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/TimerIntLdd1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/UTIL1.o: ../Generated_Code/UTIL1.c
	@echo 'Building file: $<'
	@echo 'Executing target #22 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/UTIL1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/UTIL1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/Vectors.o: ../Generated_Code/Vectors.c
	@echo 'Building file: $<'
	@echo 'Executing target #23 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Vectors.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Vectors.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/timer.o: ../Generated_Code/timer.c
	@echo 'Building file: $<'
	@echo 'Executing target #24 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/timer.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/timer.o"
	@echo 'Finished building: $<'
	@echo ' '


