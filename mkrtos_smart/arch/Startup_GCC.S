;/***************************************************************************
; * @file     Startup_GCC.S                                                 *
; * @brief    NUC980 vector table and startup code                          *
; *                                                                         *
; * SPDX-License-Identifier: Apache-2.0                                     *
; * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.       *
; ***************************************************************************/

        ARM_MODE_USER  = 0x10        /* Normal User Mode                             */
        ARM_MODE_FIQ   = 0x11        /* FIQ Fast Interrupts Mode                     */
        ARM_MODE_IRQ   = 0x12        /* IRQ Standard Interrupts Mode                 */
        ARM_MODE_SVC   = 0x13        /* Supervisor Interrupts Mode                   */
        ARM_MODE_ABORT = 0x17        /* Abort Processing memory Faults Mode          */
        ARM_MODE_UNDEF = 0x1B        /* Undefined Instructions Mode                  */
        ARM_MODE_SYS   = 0x1F        /* System Running in Priviledged Operating Mode */
        ARM_MODE_MASK  = 0x1F

        I_BIT          = 0x80        /* Disables IRQ when I bit is set               */
        F_BIT          = 0x40        /* Disables FIQ when F bit is set               */

        AIC_INTDIS0    = 0xB0042138  /* */
        AIC_INTDIS1    = 0xB004213C  /* */

        UND_Stack_Size = 0x00000100
        ABT_Stack_Size = 0x00000100
        FIQ_Stack_Size = 0x00000200
        SVC_Stack_Size = 0x00000C00
        IRQ_Stack_Size = 0x00004000
        USR_Stack_Size = 0x00004000


/*********************************************************************
*
*       Vector table
*
**********************************************************************
*/
        .text
        .global  __vector
        .global  end
        .extern  IRQ_Handler
        .extern  Reset_Handler

        .arm
        .section .vectors, "ax"
__vector:
        ldr     pc,Reset_Addr   /* RESET                 vector */
        ldr     pc,Undef_Addr   /* Undefined instruction vector */
        ldr     pc,SWI_Addr     /* Software interrupt    vector */
        ldr     pc,PAbt_Addr    /* Prefetch abort        vector */
        ldr     pc,DAbt_Addr    /* Data abort            vector */
        nop                     /* Reserved              vector */
        ldr     pc,IRQ_Addr     /* Interrupt             vector */
        ldr     pc,FIQ_Addr     /* Fast interrupt        vector */

Reset_Addr:     .word   Reset_Handler
Undef_Addr:     .word   Undef_Handler
SWI_Addr:       .word   SWI_Handler
PAbt_Addr:      .word   PAbt_Handler
DAbt_Addr:      .word   DAbt_Handler
IRQ_Addr:       .word   IRQ_Handler
FIQ_Addr:       .word   FIQ_Handler

Undef_Handler:  b       Undef_Handler
//SWI_Handler:    b       SWI_Handler
PAbt_Handler:   b       PAbt_Handler
DAbt_Handler:   b       DAbt_Handler
IRQ_Handler:    b       IRQ_Handler
FIQ_Handler:    b       FIQ_Handler
__vector_end:

Reset_Handler:

        // Disable Interrupt in case code is load by ICE while other firmware is executing
        ldr    r0, =AIC_INTDIS0
        ldr    r1, =0xFFFFFFFF
        str    r1, [r0]
        ldr    r0, =AIC_INTDIS1
        str    r1, [r0]
        //--------------------------------
        // Initial Stack Pointer register
        //--------------------------------

        //INIT_STACK
        ldr    r2, =0xB0002000
        ldr    r3,[r2,#0x0010]//��ȡsdram��С
        and    r3, r3, #0x00000007//ֻ��Ҫ����λ�����128MBYTES�ڴ�
        mov    r1,#2
        mov    r0,#1
LOOP_DRAMSIZE:
        cmp    r0,r3
        beq    DONE_DRAMSIZE
        lsl    r1,r1,#1
        add    r0,r0,#1
        b      LOOP_DRAMSIZE
DONE_DRAMSIZE:
        // Using DRAM Size to set Stack Pointer
        lsl    r0,r1,#20
        //ldr    R0, =

        // Enter Undefined Instruction Mode and set Stack Pointer
        msr    CPSR_c, #ARM_MODE_UNDEF | I_BIT | F_BIT
        MOV    SP, R0
        SUB    R0, R0, #UND_Stack_Size

        // Enter Abort Mode and set Stack Pointer
        MSR    CPSR_c, #ARM_MODE_ABORT | I_BIT | F_BIT
        MOV    SP, R0
        SUB    R0, R0, #ABT_Stack_Size

        // Enter IRQ Mode and set Stack Pointer
        MSR    CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
        MOV    SP, R0
        SUB    R0, R0, #IRQ_Stack_Size

        // Enter FIQ Mode and set Stack Pointer
        MSR    CPSR_c, #ARM_MODE_FIQ | I_BIT |F_BIT
        MOV    SP, R0
        SUB    R0, R0, #FIQ_Stack_Size

        // Enter User Mode and set Stack Pointer
        MSR    CPSR_c, #ARM_MODE_SYS | I_BIT |F_BIT
        MOV    SP, R0
        SUB    R0, R0, #USR_Stack_Size

        // Enter Supervisor Mode and set Stack Pointer
        MSR    CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
        MOV    SP, R0
        SUB    R0, R0, #SVC_Stack_Size

        MRC    p15, 0, r0 , c1, c0     /* r0 := cp15 register 1 */
        BIC    r0, r0, #0x2000         /* Clear bit13 in r1 ����δ�Ͷ�����*/
        MCR    p15, 0, r0 , c1, c0     /* cp15 register 1 := r0 */

        /*
         * copy vectors to 0x0
         */
        LDR   r1, =0x0
        LDR   r2, =__vectors_start
        LDR   r3, =__vectors_end
LoopRel:
        CMP   r2, r3
        LDRLO r0, [r2], #4
        STRLO r0, [r1], #4
        BLO   LoopRel

        /*
         * Clear .bss section
         */
        LDR   r1, =__bss_start__
        LDR   r2, =__bss_end__
        LDR   r3, =0
bss_clear_loop:
        CMP   r1, r2
        STRNE r3, [r1], #+4
        BNE   bss_clear_loop

        MOV   r0, #0                         /* No arguments are passed to main */
        MOV   r1, #0
        LDR   r2, =main
        MOV   lr, pc
        BX    r2

end:    B     end

SWI_Handler:

        STMFD   sp!,{r0-r3, r12, lr}
        MOV     r1, sp
        MRS     r0, spsr
        STMFD   sp!, {r0}
        LDRNEH  r0, [lr,#-2]
        BICNE   r0, r0, #0xFF00
        LDREQ   r0, [lr,#-4]
        BICEQ   r0, r0, #0xFF000000

        BL      C_SWI_Handler
        LDMFD   sp!, {r0}
        MSR     spsr_cf, r0
        LDMFD   sp!, {r0-r3, r12, pc}^


        .end

