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

typedef void (*outputFunctionPtr) (float pidResult);

typedef struct pid_t {
    float P;
    float I;
    float D;
    float Iterm;
    float old_pv;
    float windup;
    outputFunctionPtr write;
} pid_t;

// Function declaration
void pid_init(pid_t * pid, float P, float I, float D, float windup, outputFunctionPtr outputPtr);
void pid_calc(pid_t * pid, float sp, float pv, float deltaT);
static float constrain(float min, float max, float value);


/* [] END OF FILE */
