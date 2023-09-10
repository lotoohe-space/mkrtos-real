#if ((__ARM_ARCH_6K__ || __ARM_ARCH_6KZ__ || __ARM_ARCH_6ZK__) && !__thumb__) || __ARM_ARCH_7A__ || __ARM_ARCH_7R__ || __ARM_ARCH >= 7

#ifdef __ARM_ARCH_7M__
static inline uintptr_t __get_tp()
{
	extern unsigned long get_thread_area(void);
	return get_thread_area();
}
#else
static inline uintptr_t __get_tp()
{
	uintptr_t tp;
	__asm__("mrc p15,0,%0,c13,c0,3" : "=r"(tp));
	return tp;
}
#endif
#else

#if __ARM_ARCH_4__ || __ARM_ARCH_4T__ || __ARM_ARCH == 4
#define BLX "mov lr,pc\n\tbx"
#else
#define BLX "blx"
#endif

static inline uintptr_t __get_tp()
{
	extern hidden uintptr_t __a_gettp_ptr;
	register uintptr_t tp __asm__("r0");
	__asm__(BLX " %1" : "=r"(tp) : "r"(__a_gettp_ptr) : "cc", "lr");
	return tp;
}

#endif

#define TLS_ABOVE_TP
#define GAP_ABOVE_TP 8

#define MC_PC arm_pc
