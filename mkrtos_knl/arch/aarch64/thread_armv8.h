#pragma once

/* GPR related macros & defines */
#define CPU_GPR_COUNT 30

/* ESR_EL2 */
#define ESR_INITVAL 0x00000000LU
#define ESR_EC_MASK 0xFC000000LU
#define ESR_EC_SHIFT 26
#define ESR_IL_MASK 0x02000000LU
#define ESR_IL_SHIFT 25
#define ESR_ISS_MASK 0x01FFFFFFLU
#define ESR_ISS_SHIFT 0

/* Exception Class (EC) Values */
#define EC_UNKNOWN 0x00
#define EC_TRAP_WFI_WFE 0x01
#define EC_TRAP_MCR_MRC_CP15_A32 0x03
#define EC_TRAP_MCRR_MRRC_CP15_A32 0x04
#define EC_TRAP_MCR_MRC_CP14_A32 0x05
#define EC_TRAP_LDC_STC_CP14_A32 0x06
#define EC_SIMD_FPU 0x07
#define EC_TRAP_MRC_VMRS_CP10_A32 0x08
#define EC_TRAP_MCRR_MRRC_CP14_A32 0x0C
#define EC_TRAP_IL 0x0E
#define EC_TRAP_SVC_A32 0x11
#define EC_TRAP_HVC_A32 0x12
#define EC_TRAP_SMC_A32 0x13
#define EC_TRAP_SVC_A64 0x15
#define EC_TRAP_HVC_A64 0x16
#define EC_TRAP_SMC_A64 0x17
#define EC_TRAP_MSR_MRS_SYSTEM 0x18
#define EC_TRAP_LWREL_INST_ABORT 0x20
#define EC_CUREL_INST_ABORT 0x21
#define EC_PC_UNALIGNED 0x22
#define EC_TRAP_LWREL_DATA_ABORT 0x24
#define EC_CUREL_DATA_ABORT 0x25
#define EC_SP_UNALIGNED 0x26
#define EC_FPEXC_A32 0x28
#define EC_FPEXC_A64 0x2C
#define EC_SERROR 0x2F
#define EC_DBG_EXC_MASK 0x30

/* Instruction/Data Abort ISS Encodings */
#define ISS_ABORT_ISV_MASK 0x01000000
#define ISS_ABORT_ISV_SHIFT 24
#define ISS_ABORT_SAS_MASK 0x00C00000
#define ISS_ABORT_SAS_SHIFT 22
#define ISS_ABORT_SSE_MASK 0x00200000
#define ISS_ABORT_SSE_SHIFT 21
#define ISS_ABORT_SRT_MASK 0x001F0000
#define ISS_ABORT_SRT_SHIFT 16
#define ISS_ABORT_SF_MASK 0x00008000
#define ISS_ABORT_SF_SHIFT 15
#define ISS_ABORT_AR_MASK 0x00004000
#define ISS_ABORT_AR_SHIFT 14
#define ISS_ABORT_EA_MASK 0x00000200
#define ISS_ABORT_EA_SHIFT 9
#define ISS_ABORT_CM_MASK 0x00000100
#define ISS_ABORT_CM_SHIFT 8
#define ISS_ABORT_S1PTW_MASK 0x00000080
#define ISS_ABORT_S1PTW_SHIFT 7
#define ISS_ABORT_WNR_MASK 0x00000040
#define ISS_ABORT_WNR_SHIFT 6
#define ISS_ABORT_FSC_MASK 0x0000003F
#define ISS_ABORT_FSC_SHIFT 0

/* Fault Status (IFSC/DFSC) Encodings */
#define FSC_TRANS_FAULT_LEVEL0 0x04
#define FSC_TRANS_FAULT_LEVEL1 0x05
#define FSC_TRANS_FAULT_LEVEL2 0x06
#define FSC_TRANS_FAULT_LEVEL3 0x07
#define FSC_ACCESS_FAULT_LEVEL0 0x08
#define FSC_ACCESS_FAULT_LEVEL1 0x09
#define FSC_ACCESS_FAULT_LEVEL2 0x0A
#define FSC_ACCESS_FAULT_LEVEL3 0x0B
#define FSC_PERM_FAULT_LEVEL0 0x0C
#define FSC_PERM_FAULT_LEVEL1 0x0D
#define FSC_PERM_FAULT_LEVEL2 0x0E
#define FSC_PERM_FAULT_LEVEL3 0x0F
#define FSC_SYNC_EXTERNAL_ABORT 0x10
#define FSC_ASYNC_EXTERNAL_ABORT 0x11
#define FSC_SYNC_TWALK_EXTERNAL_ABORT_LEVEL0 0x14
#define FSC_SYNC_TWALK_EXTERNAL_ABORT_LEVEL1 0x15
#define FSC_SYNC_TWALK_EXTERNAL_ABORT_LEVEL2 0x16
#define FSC_SYNC_TWALK_EXTERNAL_ABORT_LEVEL3 0x17
#define FSC_SYNC_PARITY_ERROR 0x18
#define FSC_ASYNC_PARITY_ERROR 0x19
#define FSC_SYNC_TWALK_PARITY_ERROR_LEVEL0 0x1C
#define FSC_SYNC_TWALK_PARITY_ERROR_LEVEL1 0x1D
#define FSC_SYNC_TWALK_PARITY_ERROR_LEVEL2 0x1E
#define FSC_SYNC_TWALK_PARITY_ERROR_LEVEL3 0x1F
#define FSC_ALIGN_FAULT 0x21
#define FSC_DEBUG_EVENT 0x22
#define FSC_TLB_CONFLICT_ABORT 0x30
#define FSC_DOMAIN_FAULT_LEVEL0 0x3C
#define FSC_DOMAIN_FAULT_LEVEL1 0x3D
#define FSC_DOMAIN_FAULT_LEVEL2 0x3E
#define FSC_DOMAIN_FAULT_LEVEL3 0x3F

#define PSR_MODE32 0x00000010
#define PSR_MODE32_MASK 0x0000001f
#define PSR_MODE32_USER 0x00000010
#define PSR_MODE32_FIQ 0x00000011
#define PSR_MODE32_IRQ 0x00000012
#define PSR_MODE32_SUPERVISOR 0x00000013
#define PSR_MODE32_MONITOR 0x00000016
#define PSR_MODE32_ABORT 0x00000017
#define PSR_MODE32_HYPERVISOR 0x0000001a
#define PSR_MODE32_UNDEFINED 0x0000001b
#define PSR_MODE32_SYSTEM 0x0000001f
#define PSR_FIQ_DISABLED (1 << 6)
#define PSR_IRQ_DISABLED (1 << 7)
#define PSR_ASYNC_ABORT_DISABLED (1 << 8)
#define PSR_MODE64_DEBUG_DISABLED (1 << 9)
#define PSR_MODE32_BE_ENABLED (1 << 9)
#define PSR_IL_MASK 0x00100000
#define PSR_IL_SHIFT 20
#define PSR_SS_MASK 0x00200000
#define PSR_SS_SHIFT 21
#define PSR_OVERFLOW_MASK (1 << 28)
#define PSR_OVERFLOW_SHIFT 28
#define PSR_CARRY_MASK (1 << 29)
#define PSR_CARRY_SHIFT 29
#define PSR_ZERO_MASK (1 << 30)
#define PSR_ZERO_SHIFT 30
#define PSR_NEGATIVE_MASK (1 << 31)
#define PSR_NEGATIVE_SHIFT 31

/* Fields of AArch32-PSR which will be RES0 in Aarch64 */
/* Section 3.8.8 Unused fields of SPSR - Exception Model */
#define PSR_THUMB_ENABLED (1 << 5)
#define PSR_IT2_MASK 0x0000FC00
#define PSR_IT2_SHIFT 10
#define PSR_GE_MASK 0x000F0000
#define PSR_GE_SHIFT 16
#define PSR_JAZZLE_ENABLED (1 << 24)
#define PSR_IT1_MASK 0x06000000
#define PSR_IT1_SHIFT 25
#define PSR_CUMMULATE_MASK (1 << 27)
#define PSR_CUMMULATE_SHIFT 27

#define PSR_NZCV_MASK (PSR_NEGATIVE_MASK | \
                       PSR_ZERO_MASK |     \
                       PSR_CARRY_MASK |    \
                       PSR_OVERFLOW_MASK)
#define PSR_IT_MASK (PSR_IT2_MASK | \
                     PSR_IT1_MASK)
#define PSR_USERBITS_MASK (PSR_NZCV_MASK |      \
                           PSR_CUMMULATE_MASK | \
                           PSR_GE_MASK |        \
                           PSR_IT_MASK |        \
                           PSR_THUMB_ENABLED)
#define PSR_PRIVBITS_MASK (~PSR_USERBITS_MASK)
#define PSR_ALLBITS_MASK 0xFFFFFFFF

#define CPSR_MODE_MASK PSR_MODE32_MASK
#define CPSR_MODE_USER PSR_MODE32_USER
#define CPSR_MODE_FIQ PSR_MODE32_FIQ
#define CPSR_MODE_IRQ PSR_MODE32_IRQ
#define CPSR_MODE_SUPERVISOR PSR_MODE32_SUPERVISOR
#define CPSR_MODE_MONITOR PSR_MODE32_MONITOR
#define CPSR_MODE_ABORT PSR_MODE32_ABORT
#define CPSR_MODE_HYPERVISOR PSR_MODE32_HYPERVISOR
#define CPSR_MODE_UNDEFINED PSR_MODE32_UNDEFINED
#define CPSR_MODE_SYSTEM PSR_MODE32_SYSTEM
#define CPSR_THUMB_ENABLED PSR_THUMB_ENABLED
#define CPSR_FIQ_DISABLED PSR_FIQ_DISABLED
#define CPSR_IRQ_DISABLED PSR_IRQ_DISABLED
#define CPSR_ASYNC_ABORT_DISABLED PSR_ASYNC_ABORT_DISABLED
#define CPSR_BE_ENABLED PSR_MODE32_BE_ENABLED
#define CPSR_IT2_MASK PSR_IT2_MASK
#define CPSR_IT2_SHIFT PSR_IT2_SHIFT
#define CPSR_GE_MASK PSR_GE_MASK
#define CPSR_GE_SHIFT PSR_GE_SHIFT
#define CPSR_JAZZLE_ENABLED PSR_JAZZLE_ENABLED
#define CPSR_IT1_MASK PSR_IT1_MASK
#define CPSR_IT1_SHIFT PSR_IT1_SHIFT
#define CPSR_CUMMULATE_MASK PSR_CUMMULATE_MASK
#define CPSR_CUMMULATE_SHIFT PSR_CUMMULATE_SHIFT
#define CPSR_OVERFLOW_MASK PSR_OVERFLOW_MASK
#define CPSR_OVERFLOW_SHIFT PSR_OVERFLOW_SHIFT
#define CPSR_CARRY_MASK PSR_CARRY_MASK
#define CPSR_CARRY_SHIFT PSR_CARRY_SHIFT
#define CPSR_ZERO_MASK PSR_ZERO_MASK
#define CPSR_ZERO_SHIFT PSR_ZERO_SHIFT
#define CPSR_NEGATIVE_MASK PSR_NEGATIVE_MASK
#define CPSR_NEGATIVE_SHIFT PSR_NEGATIVE_SHIFT

#define CPSR_NZCV_MASK PSR_NZCV_MASK
#define CPSR_IT_MASK PSR_IT_MASK
#define CPSR_USERBITS_MASK PSR_USERBITS_MASK
#define CPSR_PRIVBITS_MASK PSR_PRIVBITS_MASK
#define CPSR_ALLBITS_MASK PSR_ALLBITS_MASK