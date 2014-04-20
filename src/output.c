#include "output.h"
#include <math.h>

#define DACMAX 4095

dac_t dac;

void outputCurrentToDac(float current)
{
    UB_DAC_SetDAC1(convertCurrentToDacOutput(current));
}

void outputDacCurrentfromPID(float output)
{
    if (output < 0.0f)
        output = 0.0f;
    if (output > 9.0f)
        output = 9.0f;

    output = sqrtf(output);     // make it proportional to the powe (P~I^2)

    UB_DAC_SetDAC1(convertCurrentToDacOutput(output));
}

static uint32_t convertCurrentToDacOutput(float current)
{
    if ((current >= fabsf(dac.ampereOffset)) && (current < dac.maxAmps)) {
        GPIOB->BSRRH = 0x8000;  // set PB15 low
        return lrintf((dac.ampereOffset + current) * dac.scale1ToA);
    } else {
        GPIOB->BSRRL = 0x8000;  // set PB15 high
        return 0;
    }
}
