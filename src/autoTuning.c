#include "autoTuning.h"
#include "pid.h"

static uint16_t stepCounter = 0;
static float StepResponseData[3600];


void autoTuningInitialize()
{
    stepCounter = 0;
}

void autoTuningStep(float Temperature)
{
    StepResponseData[0] = Temperature;
    stepCounter++;
}

void autoTuningReturnPidParameters(pid_t *pidresults, float AmpereInputStep)
{
    float Kpr;
    
    Kpr = (StepResponseData[stepCounter-1] - StepResponseData[0]) / AmpereInputStep;
    
    IntegrateFirst(StepResponseData, Kpr, StepResponseData[0], 1.0f, stepCounter);
    
}

static float IntegrateFirst(float *y0, float Kpr, float initialOutput, float stepTime, uint16_t steps)
{
    float result = 0;
    float y0integral;
    int i;
    
    for(i = 0; i < (steps - 1); i++) {
        y0integral += ((y0[i] + y0[i+1]) / 2) * stepTime;
        y0[i] = Kpr * stepTime * (i + 1) + (i+1) * stepTime * initialOutput - y0integral;
        result += y0[i];
    }
    
    return result;
}

static float IntegrateRest(float *YnMinusOne, float An, float stepTime, int32_t steps)
{
    float result = 0;
    float YnMinusOneIntegral = 0;
    float *Yn;
    int i;
    
    for(i = 0; i < (steps - 1); i++) {
        YnMinusOneIntegral += ((YnMinusOne[i] + YnMinusOne[i+1]) / 2) * stepTime; 
        YnMinusOne[i] = An * (i + 1) * stepTime - YnMinusOneIntegral;
        result += YnMinusOne[i];
    }
    
    return result;
}