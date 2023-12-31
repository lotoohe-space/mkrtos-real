//----------------------------------------------------------------
// Cortex-R52 Embedded example - Startup Code
//
// Copyright (c) 2016-2022 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//----------------------------------------------------------------

// MPU region defines

// Protection Region Base Address Register
#define Execute_Never 0b1         // Bit 0
#define RW_Access 0b01            // AP[2:1]
#define RO_Access 0b11
#define Non_Shareable 0b00        // SH[1:0]
#define Outer_Shareable 0x10
#define Inner_Shareable 0b11

// Protection Region Limit Address Register
#define ENable 0b1                // Bit 0
#define AttrIndx0 0b000           // AttrIndx[2:0]
#define AttrIndx1 0b001
#define AttrIndx2 0b010
#define AttrIndx3 0b011
#define AttrIndx4 0b100
#define AttrIndx5 0b101
#define AttrIndx6 0b110
#define AttrIndx7 0b111

//----------------------------------------------------------------

// Standard definitions of mode bits and interrupt (I & F) flags in PSRs
#define Mode_USR 0x10
#define Mode_FIQ 0x11
#define Mode_IRQ 0x12
#define Mode_SVC 0x13
#define Mode_MON 0x16
#define Mode_ABT 0x17
#define Mode_UND 0x1B
#define Mode_SYS 0x1F
#define Mode_HYP 0x1A

#define I_Bit 0x80 // when I bit is set, IRQ is disabled
#define F_Bit 0x40 // when F bit is set, FIQ is disabled

//----------------------------------------------------------------

    .section  VECTORS,"ax"
    .align 3
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame
    

//----------------------------------------------------------------
// Entry point for the Reset handler
//----------------------------------------------------------------

    .global Start
    .type Start, "function"

Start:

//----------------------------------------------------------------
// EL2 Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

EL2_Vectors:
        LDR PC, EL2_Reset_Addr
        LDR PC, EL2_Undefined_Addr
        LDR PC, EL2_HVC_Addr
        LDR PC, EL2_Prefetch_Addr
        LDR PC, EL2_Abort_Addr
        LDR PC, EL2_HypModeEntry_Addr
        LDR PC, EL2_IRQ_Addr
        LDR PC, EL2_FIQ_Addr


EL2_Reset_Addr:         .word    EL2_Reset_Handler
EL2_Undefined_Addr:     .word    EL2_Undefined_Handler
EL2_HVC_Addr:           .word    EL2_HVC_Handler
EL2_Prefetch_Addr:      .word    EL2_Prefetch_Handler
EL2_Abort_Addr:         .word    EL2_Abort_Handler
EL2_HypModeEntry_Addr:  .word    EL2_HypModeEntry_Handler
EL2_IRQ_Addr:           .word    EL2_IRQ_Handler
EL2_FIQ_Addr:           .word    EL2_FIQ_Handler


//----------------------------------------------------------------
// EL2 Exception Handlers
//----------------------------------------------------------------

.type EL2_Undefined_Handler, "function"
EL2_Undefined_Handler:
        B   EL2_Undefined_Handler
.type EL2_HVC_Handler, "function"
EL2_HVC_Handler:
        B   EL2_HVC_Handler
.type EL2_Prefetch_Handler, "function"
EL2_Prefetch_Handler:
        B   EL2_Prefetch_Handler
.type EL2_Abort_Handler, "function"
EL2_Abort_Handler:
        B   EL2_Abort_Handler
.type EL2_HypModeEntry_Handler, "function"
EL2_HypModeEntry_Handler:
        B   EL2_HypModeEntry_Handler
.type EL2_IRQ_Handler, "function"
EL2_IRQ_Handler:
        B   EL2_IRQ_Handler
.type EL2_FIQ_Handler, "function"
EL2_FIQ_Handler:
        B   EL2_FIQ_Handler

//----------------------------------------------------------------
// EL1 Exception Vector Table
//----------------------------------------------------------------
// Note: LDR PC instructions are used here, though branch (B) instructions
// could also be used, unless the exception handlers are >32MB away.

    .align 5
EL1_Vectors:
        LDR PC, EL1_Reset_Addr
        LDR PC, EL1_Undefined_Addr
        LDR PC, EL1_SVC_Addr
        LDR PC, EL1_Prefetch_Addr
        LDR PC, EL1_Abort_Addr
        LDR PC, EL1_Reserved
        LDR PC, EL1_IRQ_Addr
        LDR PC, EL1_FIQ_Addr


EL1_Reset_Addr:     .word    EL1_Reset_Handler
EL1_Undefined_Addr: .word    EL1_Undefined_Handler
EL1_SVC_Addr:       .word    EL1_SVC_Handler
EL1_Prefetch_Addr:  .word    EL1_Prefetch_Handler
EL1_Abort_Addr:     .word    EL1_Abort_Handler
EL1_Reserved_Addr:  .word    EL1_Reserved
EL1_IRQ_Addr:       .word    EL1_IRQ_Handler
EL1_FIQ_Addr:       .word    EL1_FIQ_Handler


//----------------------------------------------------------------
// EL1 Exception Handlers
//----------------------------------------------------------------

.type EL1_Undefined_Handler, "function"
EL1_Undefined_Handler:
        B   EL1_Undefined_Handler
.type EL1_SVC_Handler, "function"
EL1_SVC_Handler:
        B   EL1_SVC_Handler
.type EL1_Prefetch_Handler, "function"
EL1_Prefetch_Handler:
        B   EL1_Prefetch_Handler
.type EL1_Abort_Handler, "function"
EL1_Abort_Handler:
        B   EL1_Abort_Handler
EL1_Reserved:
        B   EL1_Reserved
.type EL1_IRQ_Handler, "function"
EL1_IRQ_Handler:
        B   EL1_IRQ_Handler
.type EL1_FIQ_Handler, "function"
EL1_FIQ_Handler:
        B   EL1_FIQ_Handler

//----------------------------------------------------------------
// EL2 Reset Handler
//----------------------------------------------------------------

    .section  RESET,"ax"
    .align 3

#ifdef __THUMB__
    .thumb
#endif

.type EL2_Reset_Handler, "function"
EL2_Reset_Handler:
    // Check which CPU I am
        MRC p15, 0, r0, c0, c0, 5       // Read MPIDR
        ANDS r0, r0, #0xF
        BEQ cpu0
    // If run on a multi-core system, put any secondary cores to sleep
loop_wfi:
        DSB SY      // Clear all pending data accesses
        WFI         // Go to sleep
        B loop_wfi

    // Change EL2 exception base address
cpu0:
        LDR r0, =EL2_Vectors
        MCR p15, 4, r0, c12, c0, 0      //  Write to HVBAR

    // Init HSCTLR  
        LDR r0, =0x30C5180C             // See TRM for decoding
        MCR p15, 4, r0, c1, c0, 0       // Write to HSCTLR

    // Enable EL1 access to all IMP DEF registers
        LDR r0, =0x7F81
        MCR p15, 4, r0, c1, c0, 1       // Write to HACTLR

    // Change EL1 exception base address
        LDR r0, =EL1_Vectors
        MCR p15, 0, r0, c12, c0, 0      // Write to VBAR

    // Go to SVC mode
        MRS r0, cpsr
        MOV r1, #Mode_SVC
        BFI r0, r1, #0, #5
#ifdef __THUMB__
        ORR r0, r0, #(0x1 << 5)         // Set T bit
#endif
        MSR spsr_cxsf, r0
        LDR r0, =EL1_Reset_Handler
        MSR elr_hyp, r0
        DSB
        ISB
        ERET


EL1_Reset_Handler:

//----------------------------------------------------------------
// Disable MPU and caches
//----------------------------------------------------------------

    // Disable MPU and caches in case they were left enabled from an earlier run
    // This does not need to be done from a cold reset

        MRC p15, 0, r0, c1, c0, 0       // Read System Control Register
        BIC r0, r0, #0x05               // Disable MPU (M bit) and data cache (C bit)
        BIC r0, r0, #0x1000             // Disable instruction cache (I bit)
        DSB                             // Ensure all previous loads/stores have completed
        MCR p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB                             // Ensure subsequent insts execute wrt new MPU settings


//----------------------------------------------------------------
// Cortex-R52 implementation-specific configuration
//----------------------------------------------------------------
#ifdef ENABLE_R52_SPECIFIC_CONFIG
        LDR r1,=0x3C                    // SIZE field mask

        MRC p15, 0, r0, c15, c0, 1      // Read from FLASHIFREGIONR
        ANDS r2, r0, r1                 // Extract SIZE and set flags
        BEQ 1f
        ORR r0, r0, #0x1                // Set enable bit if SIZE=!0x0
        MCR p15, 0, r0, c15, c0, 1      // Write r0 to FLASHIFREGIONR if SIZE=!0x0
1:
        MRC p15, 0, r0, c15, c0, 0      // Read from PERIPHPREGIONR
        ANDS r2, r0, r1                 // Extract SIZE and set flags
        BEQ 2f
        ORR r0, r0, #0x1                // Set enable bit if SIZE=!0x0
        MCR p15, 0, r0, c15, c0, 0      // Write r0 to PERIPHPREGIONR if SIZE=!0x0
2:
#endif

//----------------------------------------------------------------
// Stack initialization is done inside _start for all modes
// (ABT, IRQ, FIQ, UNDEF, SVC), so no need to do that here
//----------------------------------------------------------------


//----------------------------------------------------------------
// Cache invalidation. However Cortex-R52 provides CFG signals to 
// invalidate cache automatically out of reset (CFGL1CACHEINVDISx)
//----------------------------------------------------------------

        DSB             // Complete all outstanding explicit memory operations

        MOV r0, #0

        MCR p15, 0, r0, c7, c5, 0       // Invalidate entire instruction cache

        // Invalidate Data/Unified Caches

        MRC     p15, 1, r0, c0, c0, 1      // Read CLIDR
        ANDS    r3, r0, #0x07000000        // Extract coherency level
        MOV     r3, r3, LSR #23            // Total cache levels << 1
        BEQ     Finished                   // If 0, no need to clean

        MOV     r10, #0                    // R10 holds current cache level << 1
Loop1:  ADD     r2, r10, r10, LSR #1       // R2 holds cache "Set" position 
        MOV     r1, r0, LSR r2             // Bottom 3 bits are the Cache-type for this level
        AND     r1, r1, #7                 // Isolate those lower 3 bits
        CMP     r1, #2
        BLT     Skip                       // No cache or only instruction cache at this level

        MCR     p15, 2, r10, c0, c0, 0     // Write the Cache Size selection register
        ISB                                // ISB to sync the change to the CacheSizeID reg
        MRC     p15, 1, r1, c0, c0, 0      // Reads current Cache Size ID register
        AND     r2, r1, #7                 // Extract the line length field
        ADD     r2, r2, #4                 // Add 4 for the line length offset (log2 16 bytes)
        LDR     r4, =0x3FF
        ANDS    r4, r4, r1, LSR #3         // R4 is the max number on the way size (right aligned)
        CLZ     r5, r4                     // R5 is the bit position of the way size increment
        LDR     r7, =0x7FFF
        ANDS    r7, r7, r1, LSR #13        // R7 is the max number of the index size (right aligned)

Loop2:  MOV     r9, r4                     // R9 working copy of the max way size (right aligned)

#ifdef __THUMB__
Loop3:  LSL     r12, r9, r5
        ORR     r11, r10, r12              // Factor in the Way number and cache number into R11
        LSL     r12, r7, r2
        ORR     r11, r11, r12              // Factor in the Set number
#else
Loop3:  ORR     r11, r10, r9, LSL r5       // Factor in the Way number and cache number into R11
        ORR     r11, r11, r7, LSL r2       // Factor in the Set number
#endif
        MCR     p15, 0, r11, c7, c6, 2     // Invalidate by Set/Way
        SUBS    r9, r9, #1                 // Decrement the Way number
        BGE     Loop3
        SUBS    r7, r7, #1                 // Decrement the Set number
        BGE     Loop2
Skip:   ADD     r10, r10, #2               // Increment the cache number
        CMP     r3, r10
        BGT     Loop1

Finished:

//----------------------------------------------------------------
// TCM Configuration
//----------------------------------------------------------------

// Cortex-R52 optionally provides three Tightly-Coupled Memory (TCM) blocks (ATCM, BTCM and CTCM) 
//    for fast access to code or data.

// The following illustrates basic TCM configuration, as the basis for exploration by the user

#ifdef TCM

        MRC p15, 0, r0, c0, c0, 2       // Read TCM Type Register
        // r0 now contains TCM availability

        MRC p15, 0, r0, c9, c1, 0       // Read ATCM Region Register
        // r0 now contains ATCM size in bits [5:2]
        LDR r0, =Image$$ATCM$$Base      // Set ATCM base address
        ORR r0, r0, #1                  // Enable it
        MCR p15, 0, r0, c9, c1, 0       // Write ATCM Region Register

        MRC p15, 0, r0, c9, c1, 1       // Read BTCM Region Register
        // r0 now contains BTCM size in bits [5:2]
        LDR r0, =Image$$BTCM$$Base      // Set BTCM base address
        ORR r0, r0, #1                  // Enable it
        MCR p15, 0, r0, c9, c1, 1       // Write BTCM Region Register

        MRC p15, 0, r0, c9, c1, 2       // Read CTCM Region Register
        // r0 now contains CTCM size in bits [5:2]
        LDR r0, =Image$$CTCM$$Base      // Set CTCM base address
        ORR r0, r0, #1                  // Enable it
        MCR p15, 0, r0, c9, c1, 2       // Write CTCM Region Register

#endif


//----------------------------------------------------------------
// MPU Configuration
//----------------------------------------------------------------

// Notes:
// * Regions apply to both instruction and data accesses.
// * Each region base address must be a multiple of its size
// * Any address range not covered by an enabled region will abort
// * The region at 0x0 over the Vector table is needed to support semihosting

// Region 0: Code          Base = See scatter file  Limit = Based on usage   Normal  Non-shared  Read-only    Executable
// Region 1: Data          Base = See scatter file  Limit = Based on usage   Normal  Non-shared  Full access  Not Executable
// Region 2: Stack/Heap    Base = See scatter file  Limit = Based on usage   Normal  Non-shared  Full access  Not Executable
// Region 3: Peripherals   Base = 0x9A000000        Limit = 0xAFFFFFC0       Device              Full access  Not Executable
// Region 4: ATCM          Base = Configurable      Limit = Based on usage   Normal  Non-shared  Full access  Executable
// Region 5: BTCM          Base = Configurable      Limit = Based on usage   Normal  Non-shared  Full access  Executable
// Region 6: CTCM          Base = Configurable      Limit = Based on usage   Normal  Non-shared  Full access  Executable

        // Region 0 - Code
        LDR     r1, =__code_start
        LDR     r2, =((Non_Shareable<<3) | (RO_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c8, 0                   // write PRBAR0

        LDR     r1, =__code_end
        SUB     r1, r1, #1                              // convert limit from exclusive to inclusive
        BFC     r1, #0, #6                              // and clear the lower 6 bits
        LDR     r2, =((AttrIndx0<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c8, 1                   // write PRLAR0

        // Region 1 - Data
        LDR     r1, =__data_start
        LDR     r2, =((Non_Shareable<<3) | (RW_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c8, 4                   // write PRBAR1

        LDR     r1, =__data_end
        SUB     r1, r1, #1
        BFC     r1, #0, #6
        LDR     r2, =((AttrIndx0<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c8, 5                   // write PRLAR1

        // Region 2 - Stack-Heap
        LDR     r1, =__heap_start
        LDR     r2, =((Non_Shareable<<3) | (RW_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c9, 0                   // write PRBAR2

        LDR     r1, =__stack
        SUB     r1, r1, #1
        BFC     r1, #0, #6
        LDR     r2, =((AttrIndx0<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c9, 1                   // write PRLAR2

        // Region 3 - Peripherals
        LDR     r1, =0x9A000000
        LDR     r2, =((Non_Shareable<<3) | (RW_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c9, 4                   // write PRBAR3

        LDR     r1, =0xAFFFFFC0
        SUB     r1, r1, #1
        BFC     r1, #0, #6
        LDR     r2, =((AttrIndx0<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c9, 5                   // write PRLAR3

#ifdef TCM
        // Region 4 - ATCM
        LDR     r1, =Image$$ATCM$$Base
        LDR     r2, =((Non_Shareable<<3) | (RW_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c10, 0                  // write PRBAR4

        LDR     r1, =Image$$ATCM$$Limit
        SUB     r1, r1, #1
        BFC     r1, #0, #6
        LDR     r2, =((AttrIndx1<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c10, 1                  // write PRLAR4

        // Region 5 - BTCM
        LDR     r1, =Image$$BTCM$$Base
        LDR     r2, =((Non_Shareable<<3) | (RW_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c10, 4                  // write PRBAR5

        LDR     r1, =Image$$BTCM$$Limit
        SUB     r1, r1, #1
        BFC     r1, #0, #6
        LDR     r2, =((AttrIndx0<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c10, 5                  // write PRLAR5

        // Region 6 - CTCM
        LDR     r1, =Image$$CTCM$$Base
        LDR     r2, =((Non_Shareable<<3) | (RW_Access<<1))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c11, 0                  // write PRBAR6

        LDR     r1, =Image$$CTCM$$Limit
        SUB     r1, r1, #1
        BFC     r1, #0, #6
        LDR     r2, =((AttrIndx0<<1) | (ENable))
        ORR     r1, r1, r2
        MCR     p15, 0, r1, c6, c11, 1                  // write PRLAR6
#endif

    // MAIR0 configuration
        MRC p15, 0, r0, c10, c2, 0      // Read MAIR0 into r0
        LDR r1, =0xBB                   // Normal inner/outer RW cacheable, write-through
        BFI r0, r1, #0, #8              // Update Attr0
        LDR r1, =0x04                   // Device nGnRnE
        BFI r0, r1, #8, #8              // Update Attr1
        MCR p15,0,r0,c10,c2,0           // Write r0 to MAIR0

#ifdef __ARM_FP
//----------------------------------------------------------------
// Enable access to VFP by enabling access to Coprocessors 10 and 11.
// Enables Full Access i.e. in both privileged and non privileged modes
//----------------------------------------------------------------

        MRC     p15, 0, r0, c1, c0, 2      // Read Coprocessor Access Control Register (CPACR)
        ORR     r0, r0, #(0xF << 20)       // Enable access to CP 10 & 11
        MCR     p15, 0, r0, c1, c0, 2      // Write Coprocessor Access Control Register (CPACR)
        ISB

//----------------------------------------------------------------
// Switch on the VFP hardware
//----------------------------------------------------------------

        MOV     r0, #0x40000000
        VMSR    FPEXC, r0                   // Write FPEXC register, EN bit set
#endif


//----------------------------------------------------------------
// Branch to C library init
// Leaving the MPU and caches disabled until after scatter loading.
//----------------------------------------------------------------

    .global     main
        B       main


//----------------------------------------------------------------
// Enable MPU
//----------------------------------------------------------------

    .global enable_mpu
    .type enable_mpu, "function"
    .cfi_startproc
enable_mpu:
        MRC     p15, 0, r0, c1, c0, 0       // Read System Control Register
        ORR     r0, r0, #0x01               // Set M bit to enable MPU
        DSB                                 // Ensure all previous loads/stores have completed
        MCR     p15, 0, r0, c1, c0, 0       // Write System Control Register
        ISB                                 // Ensure subsequent insts execute wrt new MPU settings

        BX    lr
    .cfi_endproc


//----------------------------------------------------------------
// Enable Instruction and Data Caching
//----------------------------------------------------------------

    .global enable_caches
    .type enable_caches, "function"
    .cfi_startproc
enable_caches:
        MRC     p15, 0, r0, c1, c0, 0       // read System Control Register
        ORR     r0, r0, #(0x1 << 12)        // enable I Cache
        ORR     r0, r0, #(0x1 << 2)         // enable D Cache
        MCR     p15, 0, r0, c1, c0, 0       // write System Control Register
        ISB

        BX    lr
    .cfi_endproc

