#ifndef __VGIC_H__
#define __VGIC_H__

#include "u_types.h"

#include <errno.h>

#define VGIC_V2_MAX_LRS (1 << 6)
#define VGIC_V3_MAX_LRS 16

#define VGIC_MAX_LRS VGIC_V2_MAX_LRS

#define VGIC_V3_MAX_CPUS 255
#define VGIC_V2_MAX_CPUS 8

#define VGIC_MAX_IRQS 1024

enum vgic_type
{
    VGIC_V2, /* Good old GICv2 */
    VGIC_V3, /* New fancy GICv3 */
};

enum vgic_model_type
{
    VGIC_MODEL_V2, /* Good old GICv2 */
    VGIC_MODEL_V3, /* New fancy GICv3 */
};

#define VGIC_LR_STATE_PENDING (1 << 0)
#define VGIC_LR_STATE_ACTIVE (1 << 1)
#define VGIC_LR_STATE_MASK (3 << 0)
#define VGIC_LR_HW (1 << 2)
#define VGIC_LR_EOI_INT (1 << 3)

struct vgic_lr
{
    uint16_t virtid;
    uint16_t physid;
    uint8_t prio;
    uint8_t flags;
};

struct vgic_v2_hw_state
{
    uint32_t hcr;
    uint32_t vmcr;
    uint32_t apr;
    uint32_t lr[VGIC_V2_MAX_LRS];
};

struct vgic_v3_hw_state
{
    uint32_t hcr;
    uint32_t vmcr;
    uint32_t ap0r[4];
    uint32_t ap1r[4];
    uint64_t lr[VGIC_V3_MAX_LRS];
};

struct vgic_hw_state
{
    union
    {
        struct vgic_v2_hw_state v2;
        struct vgic_v3_hw_state v3;
    };
};

struct vgic_params
{
    enum vgic_type type;

    bool_t can_emulate_gic_v2;
    bool_t can_emulate_gic_v3;

    paddr_t vcpu_pa;

    uint32_t maint_irq;
    uint32_t lr_cnt;
};

struct vgic_ops
{
    void (*reset_state)(struct vgic_hw_state *state,
                        enum vgic_model_type model);
    void (*save_state)(struct vgic_hw_state *state,
                       enum vgic_model_type model);
    void (*restore_state)(struct vgic_hw_state *state,
                          enum vgic_model_type model);
    bool_t (*check_underflow)(void);
    void (*enable_underflow)(void);
    void (*disable_underflow)(void);
    void (*read_elrsr)(uint32_t *elrsr0, uint32_t *elrsr1);
    void (*read_eisr)(uint32_t *eisr0, uint32_t *eisr1);
    void (*set_lr)(uint32_t lr, struct vgic_lr *lrv, enum vgic_model_type model);
    void (*get_lr)(uint32_t lr, struct vgic_lr *lrv, enum vgic_model_type model);
    void (*clear_lr)(uint32_t lr);
};

int vgic_v2_probe(struct vgic_ops *ops, struct vgic_params *params);
void vgic_v2_remove(struct vgic_ops *ops, struct vgic_params *params);

int vgic_v3_probe(struct vgic_ops *ops, struct vgic_params *params);
void vgic_v3_remove(struct vgic_ops *ops, struct vgic_params *params);

#endif /* __VGIC_H__ */
