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

#include "core_cm4.h"
#include "drv_system.h"         // timers, delays, etc
#include "stm32_ub_usb_cdc.h"   // usb-serial
#include "stm32_ub_dac.h"
#include "cli.h"
#include "gpio.h"
#include "output.h"
#include "pid.h"
#include "drv_max31855.h"


#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif                          /* M_PI */

#define SENSOR1 0
#define SENSOR2 1


// main settings
typedef struct config_t {
    uint8_t manualMode;
    uint32_t cycletime;
    uint8_t debug;
} config_t;
