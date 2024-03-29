/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-11-08    MacRsh       First version
 */

#ifndef _MR_DAC_H_
#define _MR_DAC_H_

#include "mr_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MR_USING_DAC

/**
 * @brief DAC enable/disable.
 */
#define MR_DAC_DISABLE                  MR_DISABLE                  /**< DAC disabled */
#define MR_DAC_ENABLE                   MR_ENABLE                   /**< DAC enabled */

/**
 * @brief DAC configuration structure.
 */
struct mr_dac_config
{
    uint32_t enable: 1;                                             /**< DAC enable */
    uint32_t reserved: 31;
};

/**
 * @brief DAC structure.
 */
struct mr_dac
{
    struct mr_dev dev;                                              /**< Device */

    uint32_t channel;                                               /**< Channel */
};

/**
 * @brief DAC operations structure.
 */
struct mr_dac_ops
{
    int (*configure)(struct mr_dac *dac, int state);
    int (*channel_configure)(struct mr_dac *dac, int channel, int state);
    void (*write)(struct mr_dac *dac, int channel, uint32_t data);
};

/**
 * @addtogroup DAC.
 * @{
 */
int mr_dac_register(struct mr_dac *dac, const char *name, struct mr_drv *drv);
/** @} */

#endif /* MR_USING_DAC */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MR_DAC_H_ */
