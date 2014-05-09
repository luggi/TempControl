#include <stdio.h>
#include <stdint.h>

static float IntegrateFirst(float *y0, float Kpr, float initialOutput, float stepTime, uint16_t steps);
static float IntegrateRest(float *YnMinusOne, float An, float stepTime, int32_t steps);
