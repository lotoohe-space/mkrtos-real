// #ifndef _ARMV8M_MPU_H_
// #define _ARMV8M_MPU_H_

// #include <stdint.h>
// #include <stdbool.h>

// typedef struct armv8m_mpu_tag {
//     volatile uint32_t mpu_type;
//     volatile uint32_t mpu_ctrl;
//     volatile uint32_t mpu_rnr;
//     volatile uint32_t mpu_rbar;
//     volatile uint32_t mpu_rlar;
//     volatile uint32_t reserved[7];
//     volatile uint32_t mpu_mair0;
//     volatile uint32_t mpu_mair1;
// } armv8m_mpu_t;

// #define MPU_CTRL_ENABLE_SHITF       0UL
// #define MPU_CTRL_ENABLE_MASK        (1UL << MPU_CTRL_ENABLE_SHITF)

// #define MPU_CTRL_HFNMIENA_SHIFT     1UL
// #define MPU_CTRL_HFNMIENA_MASK      (1UL << MPU_CTRL_HFNMIENA_SHIFT)

// #define MPU_CTRL_PRIVDEFENA_SHIFT   2UL
// #define MPU_CTRL_PRIVDEFENA_MASK    (1UL << MPU_CTRL_PRIVDEFENA_SHIFT)

// #define MPU_RNR_REGION_SHIFT        0UL
// #define MPU_RNR_REGION_MASK         (0xFFUL << MPU_RNR_REGION_SHIFT)

// #define MPU_CTRL_RBAR_BASE_SHIFT    0x5UL
// #define MPU_CTRL_RBAR_BASE_MASK     0xFFFFFFE0UL

// #define MPU_CTRL_RBAR_SH_SHIFT      3UL
// #define MPU_CTRL_RBAR_SH_MASK       (0x3UL << MPU_CTRL_RBAR_SH_SHIFT)

// #define MPU_CTRL_RBAR_AP_SHIFT      1UL
// #define MPU_CTRL_RBAR_AP_MASK       (0x3UL << MPU_CTRL_RBAR_AP_SHIFT)

// #define MPU_CTRL_RBAR_XN_SHIFT      0UL
// #define MPU_CTRL_RBAR_XN_MASK       (1UL << MPU_CTRL_RBAR_XN_SHIFT)

// #define MPU_CTRL_RLAR_LIMIT_SHIFT   5UL
// #define MPU_CTRL_RLAR_LIMIT_MASK    0xFFFFFFE0UL

// #define MPU_CTRL_RLAR_ATTRIDX_SHIFT 1UL
// #define MPU_CTRL_RLAR_ATTRIDX_MASK  (0x7UL << MPU_CTRL_RLAR_ATTRIDX_SHIFT)

// #define MPU_CTRL_RLAR_EN_SHIFT      0UL
// #define MPU_CTRL_RLAR_EN_MASK       (1UL << MPU_CTRL_RLAR_EN_SHIFT)
// #define REGION_NON_SHAREABLE        0
// #define REGION_UNPREDICTABLE        1
// #define REGION_OUTER_SHAREABLE      2
// #define REGION_INNER_SHAREABLE      3

// #define REGION_RW_PRIV_ONLY         0
// #define REGION_RW_ANY               1
// #define REGION_RO_PRIV_ONLY         2
// #define REGION_RO_ANY               3

// #define REGION_X                    0
// #define REGION_XN                   1

// #define REGION_DISABLE              0
// #define REGION_EN                   1

// static inline void mpu_disable(armv8m_mpu_t *mpu)
// {
//     mpu->mpu_ctrl &= ~MPU_CTRL_ENABLE_MASK;
//     __asm("dsb");
//     __asm("isb");
// }

// static inline void mpu_enable(armv8m_mpu_t *mpu)
// {
//     mpu->mpu_ctrl |= MPU_CTRL_ENABLE_MASK;
//     __asm("dsb");
//     __asm("isb");
// }

// static inline void mpu_hfnmiena_enable(armv8m_mpu_t *mpu)
// {
//     mpu->mpu_ctrl |= MPU_CTRL_HFNMIENA_MASK;
// }
// static inline void mpu_hfnmiena_disable(armv8m_mpu_t *mpu)
// {
//     mpu->mpu_ctrl &= ~MPU_CTRL_HFNMIENA_MASK;
// }

// static inline void mpu_privdefena_enable(armv8m_mpu_t *mpu)
// {
//     mpu->mpu_ctrl |= MPU_CTRL_PRIVDEFENA_MASK;
// }

// static inline void mpu_privdefena_disable(armv8m_mpu_t *mpu)
// {
//     mpu->mpu_ctrl &= ~MPU_CTRL_PRIVDEFENA_MASK;
// }

// static inline void mpu_select_region(armv8m_mpu_t *mpu, uint8_t region)
// {
//     mpu->mpu_rnr |= ((region << MPU_RNR_REGION_SHIFT) & MPU_RNR_REGION_MASK);
// }

// static inline void mpu_set_region_base(armv8m_mpu_t *mpu, uint32_t base,
//                         uint8_t share_att, uint8_t ap_att, uint32_t is_xn)
// {
//     mpu->mpu_rbar = 0;
//     mpu->mpu_rbar = (base & MPU_CTRL_RBAR_BASE_MASK) |
//                     ((share_att << MPU_CTRL_RBAR_SH_SHIFT) & MPU_CTRL_RBAR_SH_MASK) |
//                     ((ap_att << MPU_CTRL_RBAR_AP_SHIFT) & MPU_CTRL_RBAR_AP_MASK) |
//                     (is_xn & MPU_CTRL_RBAR_XN_MASK);
// }

// static inline void mpu_set_region_limit(armv8m_mpu_t *mpu, uint32_t limit,
//                         uint8_t att_idx, uint8_t en)
// {
//     mpu->mpu_rlar = 0;
//     mpu->mpu_rlar = (limit & MPU_CTRL_RLAR_LIMIT_MASK) |
//                      ((att_idx << MPU_CTRL_RLAR_ATTRIDX_SHIFT) & MPU_CTRL_RLAR_ATTRIDX_MASK) |
//                      (en & MPU_CTRL_RLAR_EN_MASK);
// }

//         mpu->mpu_mair1 &= ~(0xFF << ((idx - 4) * 8));
//         mpu->mpu_mair1 |= (attr << ((idx - 4) * 8));
//     }
// }

// #endif
