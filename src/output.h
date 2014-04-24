#include "stm32f4xx.h"
#include "stm32_ub_dac.h"

// dac settings
typedef struct dac_t {
    float voltsOutput;
    float ampereOutput;
    float scale1ToA;
    float ampereOffset;
    float voltageOffset;
    float scale2ToV;
    float maxVoltage;
    float maxAmps;
} dac_t;

void outputCurrentToDac(float current);
void outputVoltageToDac(float voltage);
void outputDacCurrentfromPID(float output);

static uint32_t convertCurrentToDacOutput(float current);
static uint32_t convertVoltageToDacOutput(float voltage);
