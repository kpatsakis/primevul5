unsigned long perf_misc_flags(struct pt_regs *regs)
{
	u32 flags = perf_get_misc_flags(regs);

	if (flags)
		return flags;
	return user_mode(regs) ? PERF_RECORD_MISC_USER :
		PERF_RECORD_MISC_KERNEL;
}