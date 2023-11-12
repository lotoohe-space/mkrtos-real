/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-11-11    MacRsh       First version
 */

#include "drv_adc.h"

#ifdef MR_USING_ADC

#if !defined(MR_USING_ADC1) && !defined(MR_USING_ADC2)
#error "Please define at least one ADC macro like MR_USING_ADC1. Otherwise undefine MR_USING_ADC."
#endif

enum adc_drv_index
{
#ifdef MR_USING_ADC1
    DRV_INDEX_ADC1,
#endif /* MR_USING_ADC1 */
#ifdef MR_USING_ADC2
    DRV_INDEX_ADC2,
#endif /* MR_USING_ADC2 */
};

static const char *adc_name[] =
    {
#ifdef MR_USING_ADC1
        "adc1",
#endif /* MR_USING_ADC1 */
#ifdef MR_USING_ADC2
        "adc2",
#endif /* MR_USING_ADC2 */
    };

static struct drv_adc_channel_data adc_channel_drv_data[] =
    {
        {0,  0, 0, 0},

    };

static struct drv_adc_data adc_drv_data[] =
    {

    };

static struct mr_adc adc_dev[mr_array_num(adc_drv_data)];

static struct drv_adc_channel_data *drv_adc_get_channel_data(int channel)
{
    if (channel >= mr_array_num(adc_channel_drv_data))
    {
        return NULL;
    }
    return &adc_channel_drv_data[channel];
}

static int drv_adc_configure(struct mr_adc *adc, int state)
{

    return MR_EOK;
}

static int drv_adc_channel_configure(struct mr_adc *adc, int channel, int state)
{
  
    return MR_EOK;
}

static uint32_t drv_adc_read(struct mr_adc *adc, int channel)
{
   
    return 0;
}

static struct mr_adc_ops adc_drv_ops =
    {
        drv_adc_configure,
        drv_adc_channel_configure,
        drv_adc_read
    };

static struct mr_drv adc_drv[mr_array_num(adc_drv_data)] =
    {
#ifdef MR_USING_ADC1
        {
            Mr_Drv_Type_Adc,
            &adc_drv_ops,
            &adc_drv_data[DRV_INDEX_ADC1],
        },
#endif /* MR_USING_ADC1 */
#ifdef MR_USING_ADC2
        {
            Mr_Drv_Type_Adc,
            &adc_drv_ops,
            &adc_drv_data[DRV_INDEX_ADC2],
        },
#endif /* MR_USING_ADC2 */
    };

int drv_adc_init(void)
{
    int index = 0;

    for (index = 0; index < mr_array_num(adc_dev); index++)
    {
        mr_adc_register(&adc_dev[index], adc_name[index], &adc_drv[index]);
    }
    return MR_EOK;
}
MR_INIT_DRV_EXPORT(drv_adc_init);

#endif /* MR_USING_ADC */
