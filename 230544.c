static inline void invlpga(unsigned long addr, u32 asid)
{
	asm volatile (__ex("invlpga %1, %0") : : "c"(asid), "a"(addr));
}