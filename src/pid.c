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

void pid_calc(pid_t * pidData, float setpoint, float processValue, float deltaT)
{
    float error, output, outputNew;

    error = setpoint - processValue;

    pidData->Iterm += error * pidData->I;
    pidData->Iterm = constrain(0, pidData->windup, pidData->Iterm);

    output = (error * pidData->P) + (pidData->Iterm * deltaT) - ((processValue - pidData->old_processValue) * pidData->D / deltaT);

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
