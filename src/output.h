#include "stm32f4xx.h"
#include "stm32_ub_dac.h"

// dac settings
typedef struct dac_t {
    float voltsOutput;
    float ampereOutput;
    float scale1ToA;
    float ampereOffset;
    float scale2ToV;
    float maxAmps;
} dac_t;

void outputCurrentToDac(float current);
void outputDacCurrentfromPID(float output);

static uint32_t convertCurrentToDacOutput(float current);
