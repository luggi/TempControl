//--------------------------------------------------------------
// File     : stm32_ub_dac.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_DAC_H
#define __STM32F4_UB_DAC_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dac.h"


typedef enum {
  SINGLE_DAC1 = 0, // nur DAC1 an PA4 benutzen
  SINGLE_DAC2,     // nur DAC2 an PA5 benutzen
  DUAL_DAC         // DAC1 (PA4) und DAC2 (PA5) benutzen
}DAC_MODE_t;




//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_DAC_Init(DAC_MODE_t mode);
void UB_DAC_SetDAC1(uint16_t wert);
void UB_DAC_SetDAC2(uint16_t wert);
void UB_DAC_SetDAC1u2(uint16_t dacwert_1, uint16_t dacwert_2);


//--------------------------------------------------------------
#endif // __STM32F4_UB_DAC_H
