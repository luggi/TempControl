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
void pid_init(pid_t *pidData, float P, float I, float D, float windup, outputFunctionPtr outputPtr)
{
    pidData->P = P;
    pidData->I = I;
    pidData->D = D;
    
    pidData->windup = windup;
    pidData->Iterm = 0;
    pidData->old_pv = 0;    
    pidData->write = outputPtr;
}

void pid_calc(pid_t *pidData, float sp, float pv, float deltaT)
{
    float error, output;
    
    error = sp - pv;
    
    pidData->Iterm += error * pidData->I;
    pidData->Iterm = constrain(0, pidData->windup, pidData->Iterm);
    
    output = (error * pidData->P) + (pidData->Iterm * deltaT)  - ((pv - pidData->old_pv) * pidData->D / deltaT);
    
    pidData->old_pv = pv;
    
    pidData->write(output);
}

static float constrain(float min, float max, float value)
{
    if(value > max)
        value = max;
    else if(value < min)
        value = min;
    
    return value;
}



/* [] END OF FILE */
