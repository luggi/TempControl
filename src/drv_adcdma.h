//--------------------------------------------------------------
// File     : stm32_ub_adc2_dma.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_ADC2_DMA_H
#define __STM32F4_UB_ADC2_DMA_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"


//--------------------------------------------------------------
// Liste aller ADC-Kanäle
// (keine Nummer doppelt und von 0 beginnend)
//--------------------------------------------------------------
typedef enum {
  ADC_PA0 = 0,
  ADC_PA1 = 1,
  ADC_PA2 = 2,
  ADC_PA3 = 3
}ADC2d_NAME_t;

#define  ADC2d_ANZ   4 // Anzahl von ADC2d_NAME_t (maximum = 16)


//--------------------------------------------------------------
// Adressen der ADCs
// (siehe Seite 54+303 vom Referenz Manual)
//--------------------------------------------------------------
#define ADC_BASE_ADR        ((uint32_t)0x40012000)
#define ADC_ADC2_OFFSET     ((uint32_t)0x00000100)


//--------------------------------------------------------------
// Adressen der Register
// (siehe Seite 304+305 vom Referenz Manual)
//--------------------------------------------------------------
#define ADC_REG_DR_OFFSET         0x4C

#define ADC2_DR_ADDRESS    (ADC_BASE_ADR | ADC_ADC2_OFFSET | ADC_REG_DR_OFFSET)



//--------------------------------------------------------------
// ADC-Clock
// Max-ADC-Frq = 36MHz
// Grundfrequenz = APB2 (APB2=84MHz)
// Mögliche Vorteiler = 2,4,6,8
//--------------------------------------------------------------

//#define ADC2d_VORTEILER     ADC_Prescaler_Div2 // Frq = 42 MHz
#define ADC2d_VORTEILER     ADC_Prescaler_Div4   // Frq = 21 MHz
//#define ADC2d_VORTEILER     ADC_Prescaler_Div6 // Frq = 14 MHz
//#define ADC2d_VORTEILER     ADC_Prescaler_Div8 // Frq = 10.5 MHz


//--------------------------------------------------------------
// DMA Einstellung
// Moegliche DMAs fuer ADC2 :
//   DMA2_STREAM2_CHANNEL1
//   DMA2_STREAM3_CHANNEL1
//--------------------------------------------------------------

#define ADC2_DMA_STREAM2            2
//#define ADC2_DMA_STREAM3          3

#ifdef ADC2_DMA_STREAM2
 #define ADC2_DMA_STREAM            DMA2_Stream2
 #define ADC2_DMA_CHANNEL           DMA_Channel_1
#elif defined ADC2_DMA_STREAM3
 #define ADC2_DMA_STREAM            DMA2_Stream3
 #define ADC2_DMA_CHANNEL           DMA_Channel_1
#endif





//--------------------------------------------------------------
// Struktur eines ADC Kanals
//--------------------------------------------------------------
typedef struct {
  ADC2d_NAME_t ADC_NAME;  // Name
  GPIO_TypeDef* ADC_PORT; // Port
  const uint16_t ADC_PIN; // Pin
  const uint32_t ADC_CLK; // Clock
  const uint8_t ADC_CH;   // ADC-Kanal
}ADC2d_t;



//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_ADC2_DMA_Init(void);
uint16_t UB_ADC2_DMA_Read(ADC2d_NAME_t adc_name);


//--------------------------------------------------------------
#endif // __STM32F4_UB_ADC2_DMA_H
