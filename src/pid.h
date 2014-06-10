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
#include <stdio.h>
#include <stdint.h>
#ifndef M_PI
#define M_PI       3.14159265358979323846f
#endif /* M_PI */ 

typedef void (*outputFunctionPtr) (float pidResult);

typedef struct pid_t {
    float P;
    float I;
    float D;
    float Tf;
    float Iterm;
    float old_processValue;
    float old_output;
    float windup;
    outputFunctionPtr write;
} pid_t;

// Function declaration
void pid_init(pid_t * pid, float P, float I, float D, float Tf, float windup, outputFunctionPtr outputPtr);
void pid_calc(pid_t * pid, float sp, float pv, float deltaT);
static float constrain(float min, float max, float value);


/* [] END OF FILE */
