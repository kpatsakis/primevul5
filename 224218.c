static void print_fatal_signal(int signr)
{
	struct pt_regs *regs = signal_pt_regs();
	pr_info("potentially unexpected fatal signal %d.\n", signr);

#if defined(__i386__) && !defined(__arch_um__)
	pr_info("code at %08lx: ", regs->ip);
	{
		int i;
		for (i = 0; i < 16; i++) {
			unsigned char insn;

			if (get_user(insn, (unsigned char *)(regs->ip + i)))
				break;
			pr_cont("%02x ", insn);
		}
	}
	pr_cont("\n");
#endif
	preempt_disable();
	show_regs(regs);
	preempt_enable();
}