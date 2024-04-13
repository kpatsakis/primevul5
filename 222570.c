static __always_inline void __insn32_query(unsigned int opcode, u8 *query)
{
	register unsigned long r0 asm("0") = 0;	/* query function */
	register unsigned long r1 asm("1") = (unsigned long) query;

	asm volatile(
		/* Parameter regs are ignored */
		"	.insn	rrf,%[opc] << 16,2,4,6,0\n"
		:
		: "d" (r0), "a" (r1), [opc] "i" (opcode)
		: "cc", "memory");
}