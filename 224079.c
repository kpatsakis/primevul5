int force_sig_bnderr(void __user *addr, void __user *lower, void __user *upper)
{
	struct kernel_siginfo info;

	clear_siginfo(&info);
	info.si_signo = SIGSEGV;
	info.si_errno = 0;
	info.si_code  = SEGV_BNDERR;
	info.si_addr  = addr;
	info.si_lower = lower;
	info.si_upper = upper;
	return force_sig_info(&info);
}