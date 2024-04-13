int send_sig_mceerr(int code, void __user *addr, short lsb, struct task_struct *t)
{
	struct kernel_siginfo info;

	WARN_ON((code != BUS_MCEERR_AO) && (code != BUS_MCEERR_AR));
	clear_siginfo(&info);
	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = code;
	info.si_addr = addr;
	info.si_addr_lsb = lsb;
	return send_sig_info(info.si_signo, &info, t);
}