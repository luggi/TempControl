#include "board.h"
#include "output.h"
#include <math.h>

#define DACMAX 4095

extern config_t cfg;
extern float input_voltage[ADC_CHANNEL_COUNT];

void outputCurrentToDac(float current)
{
    if ((input_voltage[VOLTAGE_MOSFET] * input_voltage[CURRENT_MOSFET]) > cfg.output.maxPowerloss)
        current = cfg.output.maxPowerloss / input_voltage[VOLTAGE_MOSFET];
    
        UB_DAC_SetDAC1(convertCurrentToDacOutput(current));
}

void outputVoltageToDac(float voltage)
{
    UB_DAC_SetDAC2(convertVoltageToDacOutput(voltage)); // subtract offset and output scaled voltage
}

void outputCurrentfromPID1(float output)
{
    if (output < 0.0f)
        output = 0.0f;

    output = sqrtf(output);     // make current proportional to the power (P~I^2)
    
    if (cfg.pid1outputChannel == 1)
        outputCurrentToDac(output);
    else if (cfg.pid1outputChannel == 2)
        outputVoltageToDac(convertCurrentToVoltage(output));
    else
        ;
}

void outputCurrentfromPID2(float output)
{
    if (output < 0.0f)
        output = 0.0f;

    output = sqrtf(output);     // make current proportional to the power (P~I^2)
    
    if (cfg.pid2outputChannel == 1)
        outputCurrentToDac(output);
    else if (cfg.pid2outputChannel == 2)
        outputVoltageToDac(convertCurrentToVoltage(output));
    else
        ;
}

static uint32_t convertCurrentToDacOutput(float current)
{
    if ((current >= fabsf(cfg.output.ampereOffset)) && (current <= cfg.output.maxAmps)) {
        GPIOB->BSRRH = GPIO_Pin_15;  // set PB15 low
        return lrintf((cfg.output.ampereOffset + current) * cfg.output.scaleDAC1ToA);
    } else if (current > cfg.output.maxAmps) {
        GPIOB->BSRRH = GPIO_Pin_15;  // set PB15 low
        return lrintf((cfg.output.ampereOffset + cfg.output.maxAmps) * cfg.output.scaleDAC1ToA);
    } else {
        GPIOB->BSRRL = GPIO_Pin_15;  // set PB15 high
        return 0;
    }
}

static uint32_t convertVoltageToDacOutput(float voltage)
{
    if ( voltage > cfg.output.maxVoltage) {
        return 0;
    } else {
        return lrintf((cfg.output.voltageOffset + voltage) * cfg.output.scaleDAC2ToV);
    }
}

static float convertCurrentToVoltage(float current)
{
    return current * cfg.output.scaleVoltageToAmpere;
}
