#pragma once
enum Scr_bits
{
    Scr_ns = 1UL << 0,    ///< Non-Secure mode
    Scr_irq = 1UL << 1,   ///< IRQ to EL3
    Scr_fiq = 1UL << 2,   ///< FIQ to EL3
    Scr_ea = 1UL << 3,    ///< External Abort and SError to EL3
    Scr_smd = 1UL << 7,   ///< SMC disable
    Scr_hce = 1UL << 8,   ///< HVC enable at EL1, EL2, and EL3
    Scr_sif = 1UL << 9,   ///< Secure instruction fetch enable
    Scr_rw = 1UL << 10,   ///< EL2 / EL1 is AArch64
    Scr_st = 1UL << 11,   ///< Trap Secure EL1 access to timer to EL3
    Scr_twi = 1UL << 12,  ///< Trap WFI to EL3
    Scr_twe = 1UL << 13,  ///< Trap WFE to EL3
    Scr_apk = 1UL << 16,  ///< Do not trap on Pointer Authentication key accesses
    Scr_api = 1UL << 17,  ///< Do not trap on Pointer Authentication instructions
    Scr_eel2 = 1UL << 18, ///< Secure EL2 enable
};

enum
{
    Sctlr_m = 1UL << 0,
    Sctlr_a = 1UL << 1,
    Sctlr_c = 1UL << 2,
    Sctlr_sa = 1UL << 3,
    Sctlr_sa0 = 1UL << 4,
    Sctlr_cp15ben = 1UL << 5,
    Sctlr_itd = 1UL << 7,
    Sctlr_sed = 1UL << 8,
    Sctlr_uma = 1UL << 9,
    Sctlr_i = 1UL << 12,
    Sctlr_dze = 1UL << 14,
    Sctlr_uct = 1UL << 15,
    Sctlr_ntwi = 1UL << 16,
    Sctlr_ntwe = 1UL << 18,
    Sctlr_wxn = 1UL << 19,
    Sctlr_e0e = 1UL << 24,
    Sctlr_ee = 1UL << 25,
    Sctlr_uci = 1UL << 26,

    Sctlr_el1_res = (1UL << 11) | (1UL << 20) | (3UL << 22) | (3UL << 28),

    Sctlr_el1_generic = Sctlr_c | Sctlr_cp15ben | Sctlr_i | Sctlr_dze | Sctlr_uct | Sctlr_uci | Sctlr_el1_res,
};
enum
{
    Scr_default_bits = Scr_ns | Scr_rw | Scr_smd | Scr_hce,
};

enum
{
    Hcr_vm = 1UL << 0,      ///< Virtualization enable
    Hcr_swio = 1UL << 1,    ///< Set/way invalidation override
    Hcr_ptw = 1UL << 2,     ///< Protected table walk
    Hcr_fmo = 1UL << 3,     ///< Physical FIQ routing
    Hcr_imo = 1UL << 4,     ///< Physical IRQ routing
    Hcr_amo = 1UL << 5,     ///< Physical SError interrupt routing
    Hcr_dc = 1UL << 12,     ///< Default cacheability
    Hcr_tid2 = 1UL << 17,   ///< Trap CTR, CESSLR, etc.
    Hcr_tid3 = 1UL << 18,   ///< Trap ID, etc.
    Hcr_tsc = 1UL << 19,    ///< Trap SMC instructions
    Hcr_tidcp = 1UL << 20,  ///< Trap implementation defined functionality
    Hcr_tactlr = 1UL << 21, ///< Trap ACTLR, etc.
    Hcr_tsw = 1UL << 22,    ///< Trap cache maintenance instructions
    Hcr_ttlb = 1UL << 25,   ///< Trap TLB maintenance instructions
    Hcr_tvm = 1UL << 26,    ///< Trap virtual memory controls
    Hcr_tge = 1UL << 27,    ///< Trap General Exceptions
    Hcr_hcd = 1UL << 29,    ///< HVC instruction disable
    Hcr_trvm = 1UL << 30,   ///< Trap reads of virtual memory controls
    Hcr_rw = 1UL << 31,     ///< EL1 is AArch64
};

enum
{
    Hcr_must_set_bits = Hcr_vm | Hcr_swio | Hcr_ptw | Hcr_amo | Hcr_imo | Hcr_fmo | Hcr_tidcp | Hcr_tsc | Hcr_tactlr,

    /**
     * HCR value to be used for the VMM.
     *
     * The AArch64 VMM is currently running in EL1.
     */
    Hcr_host_bits = Hcr_must_set_bits | Hcr_rw | Hcr_dc,

    /**
     * HCR value to be used for normal threads.
     *
     * On AArch64 (with virtualization support) running in EL1.
     */
    Hcr_non_vm_bits = Hcr_must_set_bits | Hcr_rw | Hcr_dc | Hcr_tsw | Hcr_ttlb | Hcr_tvm | Hcr_trvm
};

enum
{
    Mdcr_hpmn_mask = 0xf,
    Mdcr_tpmcr = 1UL << 5,
    Mdcr_tpm = 1UL << 6,
    Mdcr_hpme = 1UL << 7,
    Mdcr_tde = 1UL << 8,
    Mdcr_tda = 1UL << 9,
    Mdcr_tdosa = 1UL << 10,
    Mdcr_tdra = 1UL << 11,

    Mdcr_bits = Mdcr_tpmcr | Mdcr_tpm | Mdcr_tda | Mdcr_tdosa | Mdcr_tdra,
    Mdcr_vm_mask = 0xf00,
};
enum
{
    /// Attributes for page-table walks
    Tcr_attribs = (3UL << 4)    // SH0
                  | (1UL << 2)  // ORGN0
                  | (1UL << 0), // IRGN0

    /**
     * Memory Attribute Indirection (MAIR0)
     * Attr0: Device-nGnRnE memory
     * Attr1: Normal memory, Inner/Outer Non-cacheable
     * Attr2: Normal memory, RW, Inner/Outer Write-Back Cacheable (Non-transient)
     */
    Mair0_prrr_bits = 0x00ff4400,
    Mair1_nmrr_bits = 0,
};
enum
{
    Vtcr_bits = (1UL << 6)    // SL0
                | (2UL << 16) // PS
                | (25UL << 0) // T0SZ
};
enum Hstr_values
{
    Hstr_non_vm = 0x9f6f, // ALL but crn=13,7 (TPIDxxR, DSB) CP15 traped
    Hstr_vm = 0x0,        // none
};
void init_arm_hyp(void);
