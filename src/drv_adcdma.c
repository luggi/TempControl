//--------------------------------------------------------------
// File     : stm32_ub_adc2_dma.c
// Datum    : 04.03.2013
// Version  : 1.1
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO,ADC,DMA
// Funktion : AD-Wandler (ADC2 per DMA2)
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "drv_adcdma.h"


//--------------------------------------------------------------
// Definition der benutzten ADC Pins (max=16)
// Reihenfolge wie bei ADC2d_NAME_t
//--------------------------------------------------------------
ADC2d_t ADC2d[] = {
  //NAME  ,PORT , PIN      , CLOCK              , Kanal
  {ADC_PA0, GPIOA, GPIO_Pin_0, RCC_AHB1Periph_GPIOA, ADC_Channel_0},
  {ADC_PA1, GPIOA, GPIO_Pin_1, RCC_AHB1Periph_GPIOC, ADC_Channel_1},
  {ADC_PA2, GPIOA, GPIO_Pin_2, RCC_AHB1Periph_GPIOC, ADC_Channel_2},
  {ADC_PA3, GPIOA, GPIO_Pin_3, RCC_AHB1Periph_GPIOC, ADC_Channel_3},
};



//--------------------------------------------------------------
// Globale Variabeln
//--------------------------------------------------------------
volatile uint32_t ADC2_DMA_Buffer[ADC2d_ANZ];  // Globaler ADC-Puffer


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_ADC2d_InitIO(void);
void P_ADC2d_InitDMA(void);
void P_ADC2d_InitADC(void);
void P_ADC2d_Start(void);


//--------------------------------------------------------------
// init vom ADC2 im DMA Mode
// und starten der zyklischen Wandlung
//--------------------------------------------------------------
void UB_ADC2_DMA_Init(void)
{
  P_ADC2d_InitIO();
  P_ADC2d_InitDMA();
  P_ADC2d_InitADC();
  P_ADC2d_Start();
}

//--------------------------------------------------------------
// auslesen eines Messwertes
//--------------------------------------------------------------
uint16_t UB_ADC2_DMA_Read(ADC2d_NAME_t adc_name)
{
  uint16_t messwert=0;

  messwert=(uint16_t)(ADC2_DMA_Buffer[adc_name]);

  return(messwert);
}




//--------------------------------------------------------------
// interne Funktion
// Init aller IO-Pins
//--------------------------------------------------------------
void P_ADC2d_InitIO(void) {
  GPIO_InitTypeDef      GPIO_InitStructure;
  int adc_name;

  for(adc_name = 0; adc_name < ADC2d_ANZ; adc_name++) {
    // Clock Enable
    RCC_AHB1PeriphClockCmd(ADC2d[adc_name].ADC_CLK, ENABLE);

    // Config des Pins als Analog-Eingang
    GPIO_InitStructure.GPIO_Pin = ADC2d[adc_name].ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(ADC2d[adc_name].ADC_PORT, &GPIO_InitStructure);
  }
}

//--------------------------------------------------------------
// interne Funktion
// Init von DMA Nr.2
//--------------------------------------------------------------
void P_ADC2d_InitDMA(void)
{
  DMA_InitTypeDef       DMA_InitStructure;

  // Clock Enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  // DMA-Disable
  DMA_Cmd(ADC2_DMA_STREAM, DISABLE);
  DMA_DeInit(ADC2_DMA_STREAM);

  // DMA2-Config
  DMA_InitStructure.DMA_Channel = ADC2_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC2_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC2_DMA_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = ADC2d_ANZ;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(ADC2_DMA_STREAM, &DMA_InitStructure);
  DMA_Cmd(ADC2_DMA_STREAM, ENABLE);
}

//--------------------------------------------------------------
// interne Funktion
// Init von ADC Nr.2
//--------------------------------------------------------------
void P_ADC2d_InitADC(void)
{
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef       ADC_InitStructure;
  int adc_name;

  // Clock Enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

  // ADC-Config
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC2d_VORTEILER;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = ADC2d_ANZ;
  ADC_Init(ADC2, &ADC_InitStructure);

  // hinzufuegen aller ADC-Kanäle in die Regular-Group
  for(adc_name=0;adc_name<ADC2d_ANZ;adc_name++) {
    ADC_RegularChannelConfig(ADC2, ADC2d[adc_name].ADC_CH, adc_name+1, ADC_SampleTime_28Cycles);
  }
}


//--------------------------------------------------------------
// interne Funktion
// Enable und start vom ADC und DMA
//--------------------------------------------------------------
void P_ADC2d_Start(void)
{
  ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
  ADC_DMACmd(ADC2, ENABLE);
  ADC_Cmd(ADC2, ENABLE);
  ADC_SoftwareStartConv(ADC2);
}


