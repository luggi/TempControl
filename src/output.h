#ifndef OUTPUT_H_
#define OUTPUT_H_ 

#include "stm32f4xx.h"
#include "drv_dac.h"

// dac settings
typedef struct output_t {
    float voltageOutput;
    float externalAmpereOutput;
    float internalAmpereOutput;
    float ampereOffset;
    float voltageOffset;
    float scaleDAC1ToA;
    float scaleDAC2ToV;
    float scaleVoltageToAmpere;
    float maxPowerloss;
    float maxVoltage;
    float maxAmps;
} output_t;

void outputCurrentToDac(float current);
void outputVoltageToDac(float voltage);
void outputCurrentfromPID1(float output);
void outputCurrentfromPID2(float output);

static uint32_t convertCurrentToDacOutput(float current);
static uint32_t convertVoltageToDacOutput(float voltage);
static float convertCurrentToVoltage(float current);

#endif
