static bool regs_use_siar(struct pt_regs *regs)
{
	/*
	 * When we take a performance monitor exception the regs are setup
	 * using perf_read_regs() which overloads some fields, in particular
	 * regs->result to tell us whether to use SIAR.
	 *
	 * However if the regs are from another exception, eg. a syscall, then
	 * they have not been setup using perf_read_regs() and so regs->result
	 * is something random.
	 */
	return ((TRAP(regs) == INTERRUPT_PERFMON) && regs->result);
}