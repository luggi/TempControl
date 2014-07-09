#pragma once

// for roundf()
#define __USE_C99_MATH

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"          // Device header
#include "stm32f4xx_flash.h"

#include "core_cm4.h"
#include "drv_system.h"         // timers, delays, etc
#include "stm32_ub_usb_cdc.h"   // usb-serial
#include "drv_dac.h"
#include "cli.h"
#include "gpio.h"
#include "output.h"
#include "pid.h"
#include "drv_max31855.h"
#include "drv_adcdma.h"
#include "config.h"


#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif                          /* M_PI */

#define MAX_TEMP_VARIATION 15.0f

enum sensors {
    SENSOR1 = 0,
    SENSOR2,
    TEMPERATURE_SENSOR_COUNT
};

enum pids {
    PID1 = 0,
    PID2,
    PID_CONTROLLER_COUNT
};

enum outputs {
    INTERNAL_CURRENT,
    EXTERNAL_CURRENT,
    OFF
};

enum inputs {
    VOLTAGE_MOSFET = 0,
    CURRENT_MOSFET,
    VOLTAGE_INPUT,
    VOLTAGE_VMON,
    CURRENT_CMON,
    ADC_CHANNEL_COUNT
};

typedef struct control_t {
    float temperature[TEMPERATURE_SENSOR_COUNT];
    float temperature_smooth[TEMPERATURE_SENSOR_COUNT];
    float setpoint[PID_CONTROLLER_COUNT];
} control_t;

typedef struct input_t {
    float scale[ADC_CHANNEL_COUNT];
} input_t;

// main settings
typedef struct config_t {
    uint8_t magic_be;
    uint8_t magic_ef;
    int8_t eeprom;
    uint8_t manualMode;
    uint32_t cycletime;
    uint8_t debug;
    uint8_t pid1outputChannel;
    uint8_t pid2outputChannel;
    pid_t pid1;
    pid_t pid2;
    output_t output;
    input_t input;
    float temperature_averaging;
} config_t;


