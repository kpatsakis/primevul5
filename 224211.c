int force_sig_pkuerr(void __user *addr, u32 pkey)
{
	struct kernel_siginfo info;

	clear_siginfo(&info);
	info.si_signo = SIGSEGV;
	info.si_errno = 0;
	info.si_code  = SEGV_PKUERR;
	info.si_addr  = addr;
	info.si_pkey  = pkey;
	return force_sig_info(&info);
}