static bool ex_handler_bpf(const struct exception_table_entry *x,
			   struct pt_regs *regs, int trapnr,
			   unsigned long error_code, unsigned long fault_addr)
{
	u32 reg = x->fixup >> 8;

	/* jump over faulting load and clear dest register */
	*(unsigned long *)((void *)regs + reg) = 0;
	regs->ip += x->fixup & 0xff;
	return true;
}