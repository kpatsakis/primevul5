static int post_copy_siginfo_from_user32(kernel_siginfo_t *to,
					 const struct compat_siginfo *from)
{
	clear_siginfo(to);
	to->si_signo = from->si_signo;
	to->si_errno = from->si_errno;
	to->si_code  = from->si_code;
	switch(siginfo_layout(from->si_signo, from->si_code)) {
	case SIL_KILL:
		to->si_pid = from->si_pid;
		to->si_uid = from->si_uid;
		break;
	case SIL_TIMER:
		to->si_tid     = from->si_tid;
		to->si_overrun = from->si_overrun;
		to->si_int     = from->si_int;
		break;
	case SIL_POLL:
		to->si_band = from->si_band;
		to->si_fd   = from->si_fd;
		break;
	case SIL_FAULT:
		to->si_addr = compat_ptr(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		to->si_trapno = from->si_trapno;
#endif
		break;
	case SIL_FAULT_MCEERR:
		to->si_addr = compat_ptr(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		to->si_trapno = from->si_trapno;
#endif
		to->si_addr_lsb = from->si_addr_lsb;
		break;
	case SIL_FAULT_BNDERR:
		to->si_addr = compat_ptr(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		to->si_trapno = from->si_trapno;
#endif
		to->si_lower = compat_ptr(from->si_lower);
		to->si_upper = compat_ptr(from->si_upper);
		break;
	case SIL_FAULT_PKUERR:
		to->si_addr = compat_ptr(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		to->si_trapno = from->si_trapno;
#endif
		to->si_pkey = from->si_pkey;
		break;
	case SIL_CHLD:
		to->si_pid    = from->si_pid;
		to->si_uid    = from->si_uid;
		to->si_status = from->si_status;
#ifdef CONFIG_X86_X32_ABI
		if (in_x32_syscall()) {
			to->si_utime = from->_sifields._sigchld_x32._utime;
			to->si_stime = from->_sifields._sigchld_x32._stime;
		} else
#endif
		{
			to->si_utime = from->si_utime;
			to->si_stime = from->si_stime;
		}
		break;
	case SIL_RT:
		to->si_pid = from->si_pid;
		to->si_uid = from->si_uid;
		to->si_int = from->si_int;
		break;
	case SIL_SYS:
		to->si_call_addr = compat_ptr(from->si_call_addr);
		to->si_syscall   = from->si_syscall;
		to->si_arch      = from->si_arch;
		break;
	}
	return 0;
}