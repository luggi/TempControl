//--------------------------------------------------------------
// File     : stm32_ub_dac.c
// Datum    : 23.03.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : DAC
// Funktion : DA-Wandler (DAC1 und DAC2)
//
// Hinweis  : DAC1-Ausgang an PA4
//            DAC2-Ausgang an PA5
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "drv_dac.h"


//--------------------------------------------------------------
// Globale Variablen
//--------------------------------------------------------------
DAC_MODE_t akt_dac_mode;


//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void P_DAC1_InitIO(void);
void P_DAC1_InitDAC(void);
void P_DAC2_InitIO(void);
void P_DAC2_InitDAC(void);


//--------------------------------------------------------------
// init vom DA-Wandler
// mode : [SINGLE_DAC1, SINGLE_DAC2, DUAL_DAC]
//--------------------------------------------------------------
void UB_DAC_Init(DAC_MODE_t mode)
{
    // DAC-Mode speichern
    akt_dac_mode = mode;

    if ((mode == SINGLE_DAC1) || (mode == DUAL_DAC)) {
        P_DAC1_InitIO();
        P_DAC1_InitDAC();
        // DAC1 auf 0 setzen
        UB_DAC_SetDAC1(0);
    }
    if ((mode == SINGLE_DAC2) || (mode == DUAL_DAC)) {
        P_DAC2_InitIO();
        P_DAC2_InitDAC();
        // DAC2 auf 0 setzen
        UB_DAC_SetDAC2(0);
    }
}


//--------------------------------------------------------------
// Stellt einen DAC-Wert am DAC1 ein
// Wert  : 0 bis 4095
//--------------------------------------------------------------
void UB_DAC_SetDAC1(uint16_t wert)
{
    if (akt_dac_mode == SINGLE_DAC2)
        return;                 // wenn nicht erlaubt

    // Maxwert = 12bit
    if (wert > 4095)
        wert = 4095;

    // DAC1 Wert einstellen (12Bit, rechtsbündig)
    DAC_SetChannel1Data(DAC_Align_12b_R, wert);
}


//--------------------------------------------------------------
// Stellt einen DAC-Wert am DAC2 ein
// Wert  : 0 bis 4095
//--------------------------------------------------------------
void UB_DAC_SetDAC2(uint16_t wert)
{
    if (akt_dac_mode == SINGLE_DAC1)
        return;                 // wenn nicht erlaubt

    // Maxwert = 12bit
    if (wert > 4095)
        wert = 4095;

    // DAC2 Wert einstellen (12Bit, rechtsbündig)
    DAC_SetChannel2Data(DAC_Align_12b_R, wert);
}


//--------------------------------------------------------------
// Stellt gleichzeitig einen DAC-Wert am DAC1 und DAC2 ein
// dacwert  : 0 bis 4095
//--------------------------------------------------------------
void UB_DAC_SetDAC1u2(uint16_t dacwert_1, uint16_t dacwert_2)
{
    if (akt_dac_mode != DUAL_DAC)
        return;                 // wenn nicht erlaubt

    // Maxwert = 12bit
    if (dacwert_1 > 4095)
        dacwert_1 = 4095;
    if (dacwert_2 > 4095)
        dacwert_2 = 4095;

    // DAC1 und DAC2 Wert einstellen (12Bit, rechtsbündig)
    DAC_SetDualChannelData(DAC_Align_12b_R, dacwert_2, dacwert_1);
}


//--------------------------------------------------------------
// interne Funktion
// Init vom IO-Pin für DAC1
//--------------------------------------------------------------
void P_DAC1_InitIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Clock Enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Config des DAC-Pins als Analog-Ausgang
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//--------------------------------------------------------------
// interne Funktion
// Init vom IO-Pin für DAC2
//--------------------------------------------------------------
void P_DAC2_InitIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Clock Enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Config des DAC-Pins als Analog-Ausgang
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//--------------------------------------------------------------
// interne Funktion
// Init vom DAC1
//--------------------------------------------------------------
void P_DAC1_InitDAC(void)
{
    DAC_InitTypeDef DAC_InitStructure;

    // Clock Enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // DAC-Config
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // DAC enable
    DAC_Cmd(DAC_Channel_1, ENABLE);
}


//--------------------------------------------------------------
// interne Funktion
// Init vom DAC2
//--------------------------------------------------------------
void P_DAC2_InitDAC(void)
{
    DAC_InitTypeDef DAC_InitStructure;

    // Clock Enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // DAC-Config
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    // DAC enable
    DAC_Cmd(DAC_Channel_2, ENABLE);
}
