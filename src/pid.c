/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "pid.h"

/* 
Pid initialisation 
*/
void pid_init(pid_t * pidData, float P, float I, float D, float Tf, float windup, outputFunctionPtr outputPtr)
{
    pidData->P = P;
    pidData->I = I;
    pidData->D = D;

    pidData->windup = windup;
    pidData->Iterm = 0;
    pidData->old_processValue = 0;
    pidData->old_output = 0;
    pidData->write = outputPtr;
    pidData->Tf = Tf;
}
#define F_CUT_ACCZ 0.2f // 0.2Hz -> ~5 seconds weighted averaging
static const float fc_dterm = 0.5f / (M_PI * F_CUT_ACCZ);

void pid_calc(pid_t * pidData, float setpoint, float processValue, float deltaT)
{
    float error, output, outputNew, dTerm;
    static float dTerm_filtered = 0;

    error = setpoint - processValue;

    pidData->Iterm += error * pidData->I;
    pidData->Iterm = constrain(0, pidData->windup, pidData->Iterm);
    
    dTerm = ((processValue - pidData->old_processValue) * pidData->D / deltaT);
    dTerm_filtered = dTerm_filtered + (deltaT / (fc_dterm + deltaT)) * (dTerm - dTerm_filtered);

    output = (error * pidData->P) + (pidData->Iterm * deltaT) - dTerm_filtered;

    pidData->old_processValue = processValue;
    
    if(pidData->Tf > 0) // apply an output filter if Tf is set
        outputNew = pidData->old_output + (deltaT/pidData->Tf) * (output - pidData->old_output);
    else outputNew = output;

    pidData->write(outputNew);
}

static float constrain(float min, float max, float value)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;

    return value;
}



/* [] END OF FILE */
