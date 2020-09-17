################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/decodificacion.c" \
"../Sources/i2c.c" \
"../Sources/main.c" \
"../Sources/utils.c" \

C_SRCS += \
../Sources/decodificacion.c \
../Sources/i2c.c \
../Sources/main.c \
../Sources/utils.c \

OBJS += \
./Sources/decodificacion_c.obj \
./Sources/i2c_c.obj \
./Sources/main_c.obj \
./Sources/utils_c.obj \

OBJS_QUOTED += \
"./Sources/decodificacion_c.obj" \
"./Sources/i2c_c.obj" \
"./Sources/main_c.obj" \
"./Sources/utils_c.obj" \

C_DEPS += \
./Sources/decodificacion_c.d \
./Sources/i2c_c.d \
./Sources/main_c.d \
./Sources/utils_c.d \

C_DEPS_QUOTED += \
"./Sources/decodificacion_c.d" \
"./Sources/i2c_c.d" \
"./Sources/main_c.d" \
"./Sources/utils_c.d" \

OBJS_OS_FORMAT += \
./Sources/decodificacion_c.obj \
./Sources/i2c_c.obj \
./Sources/main_c.obj \
./Sources/utils_c.obj \


# Each subdirectory must supply rules for building sources it contributes
Sources/decodificacion_c.obj: ../Sources/decodificacion.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/decodificacion.args" -ObjN="Sources/decodificacion_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/%.d: ../Sources/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

Sources/i2c_c.obj: ../Sources/i2c.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/i2c.args" -ObjN="Sources/i2c_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/main_c.obj: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/main.args" -ObjN="Sources/main_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '

Sources/utils_c.obj: ../Sources/utils.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: HCS08 Compiler'
	"$(HC08ToolsEnv)/chc08" -ArgFile"Sources/utils.args" -ObjN="Sources/utils_c.obj" "$<" -Lm="$(@:%.obj=%.d)" -LmCfg=xilmou
	@echo 'Finished building: $<'
	@echo ' '


