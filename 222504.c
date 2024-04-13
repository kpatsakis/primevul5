static inline int plo_test_bit(unsigned char nr)
{
	register unsigned long r0 asm("0") = (unsigned long) nr | 0x100;
	int cc;

	asm volatile(
		/* Parameter registers are ignored for "test bit" */
		"	plo	0,0,0,0(0)\n"
		"	ipm	%0\n"
		"	srl	%0,28\n"
		: "=d" (cc)
		: "d" (r0)
		: "cc");
	return cc == 0;
}