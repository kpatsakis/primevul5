int send_sig_fault(int sig, int code, void __user *addr
	___ARCH_SI_TRAPNO(int trapno)
	___ARCH_SI_IA64(int imm, unsigned int flags, unsigned long isr)
	, struct task_struct *t)
{
	struct kernel_siginfo info;

	clear_siginfo(&info);
	info.si_signo = sig;
	info.si_errno = 0;
	info.si_code  = code;
	info.si_addr  = addr;
#ifdef __ARCH_SI_TRAPNO
	info.si_trapno = trapno;
#endif
#ifdef __ia64__
	info.si_imm = imm;
	info.si_flags = flags;
	info.si_isr = isr;
#endif
	return send_sig_info(info.si_signo, &info, t);
}