
#include "adc.h"
#include "temp_cal.h"
#include "sysinfo.h"
#include "u_sys.h"
#include "relay.h"
#include <math.h>
#define TEMP_ADJUST_TIMES 6000 // ms
// Rt = R *EXP(B*(1/T1-1/T2))
// 这里T1和T2指的是K度即开尔文温度，K度=273.15(绝对温度)+摄氏度；其中T2=(273.15+25)
// Rt 是热敏电阻在T1温度下的阻值；
// R是热敏电阻在T2常温下的标称阻值；
// B值是热敏电阻的重要参数；
// EXP是e的n次方；

const float Rp = 10000.0;         // 10K
const float T2 = (273.15 + 25.0); // T2
const float Bx = 3950.0;          // B
const float Ka = 273.15;

// 根据电阻值计算温度值
float get_temp(int resistor)
{
    float Rt;
    float temp;

    Rt = resistor;
    // like this R=5000, T2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25)),
    temp = Rt / Rp;
    temp = log(temp); // ln(Rt/Rp)
    temp /= Bx;       // ln(Rt/Rp)/B
    temp += (1 / T2);
    temp = 1 / (temp);
    temp -= Ka;
    return temp;
}

#define RES_CAL(a) ((double)((a)) / (((3.3 - a) / 10000.0)))

#define TEMPS_NUMBET 4 // 六路温度

#define TEP_CAP_COUNT 50 // 15次平均

u32 adc_cap_val[TEMPS_NUMBET] = {0};
u16 adc_count = 0;
void temps_cal(void)
{
    u8 i;
    for (i = 0; i < TEMPS_NUMBET; i++)
    {
        adc_cap_val[i] += ADCConvertedValue[i];
    }
    adc_count++;
    if (adc_count >= TEP_CAP_COUNT)
    {
        adc_count = 0;
        for (i = 0; i < TEMPS_NUMBET; i++)
        {
            adc_cap_val[i] /= TEP_CAP_COUNT;
        }
        sys_info.temp[0] = get_temp(RES_CAL(((double)adc_cap_val[0] / 4096.) * 3.3));
        sys_info.temp[1] = get_temp(RES_CAL(((double)adc_cap_val[1] / 4096.) * 3.3));
        sys_info.temp[2] = get_temp(RES_CAL(((double)adc_cap_val[2] / 4096.) * 3.3));
        sys_info.temp[3] = get_temp(RES_CAL(((double)adc_cap_val[3] / 4096.) * 3.3));
        sys_info.temp[4] = get_temp(RES_CAL(((double)adc_cap_val[4] / 4096.) * 3.3));
        sys_info.temp[5] = get_temp(RES_CAL(((double)adc_cap_val[5] / 4096.) * 3.3));
        for (i = 0; i < TEMPS_NUMBET; i++)
        {
            if (sys_info.temp[i] < TEMP_MIN_VAL)
            {
                sys_info.temp[i] = TEMP_MIN_VAL;
            }
            else if (sys_info.temp[i] > TEMP_MAX_VAL)
            {
                sys_info.temp[i] = TEMP_MAX_VAL;
            }
        }
        for (i = 0; i < TEMPS_NUMBET; i++)
        {
            adc_cap_val[i] = 0;
        }
    }
}
static u32 temp_adjust_tick_last_update = 0;
void temp_cal(void)
{

    temps_cal(); // 计算温度值 8路

    // 调温处理
    // 温度处理
    if (sys_read_tick() - temp_adjust_tick_last_update >= TEMP_ADJUST_TIMES)
    {
        temp_adjust_tick_last_update = sys_read_tick();
        if ((int)(sys_info.temp[0]) > sys_info.target_val)
        {
            relay_ctrl(5, 0);
        }
        else
        {
            relay_ctrl(5, 1);
        }
    }
}