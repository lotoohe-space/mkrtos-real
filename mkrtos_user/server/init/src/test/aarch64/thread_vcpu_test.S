.text
.global thread_vcpu_test_fn
.type   thread_vcpu_test_fn,%function
thread_vcpu_test_fn:
    //mrs x0, ttbr0_el1
    mov x0,x0
    tlbi	vmalle1	// Invalidate local TLB
	dsb	nsh
    mov x0, #0x30000000
    msr vbar_el1, x0
    mov x0, #0x40000000
    msr ttbr0_el1, x0
    msr ttbr1_el1, x0
    isb
    mov x0, #0
    msr spsr_el1, x0

    mov x0, #1
    mrs x1, SCTLR_EL1
    orr x1, x1, x0
    msr SCTLR_EL1, x1
    isb
    ic iallu
    dsb nsh
    isb
    mrs x0, CurrentEL
    
    mov x0, #0
    msr spsr_el1, x0
    adr x0, el0_entry
   // mov x0, #0x80000
    msr elr_el1, x0
    eret
el0_entry:
    //mrs x0, sp_el0
    mov x0, #0x10000000
    str x1, [x0]
    mrs x0, TTBR0_EL2
    mov x0, x1
    mov x0, x1
    mov x0, x1
    mov x0, x1
    mov x0, x1
    mov x0, x1
    mov x0, x1
    // wfe //!<陷入内核
    // wfi

    b .



