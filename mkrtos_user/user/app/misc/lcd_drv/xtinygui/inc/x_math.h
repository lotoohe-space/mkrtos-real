#pragma once
#include <math.h>

float x_atan2_approx(float y,float x);
float x_atan2_PI4(float y,float x);
float x_atan2_LUT(float y,float x);
float x_atan2_LUT_if(float y,float x);

float x_sin(float x);
float x_cos(float a);

static inline float x_atan(float y, float x)
{
    if (x >= 0)
        return x_atan2_LUT(y, x);
    else if (y >= 0)
        return x_atan2_LUT(y, x) - M_PI;
    else
        return x_atan2_LUT(y, x) + M_PI;
}
