int __copy_siginfo_to_user32(struct compat_siginfo __user *to,
			     const struct kernel_siginfo *from, bool x32_ABI)
#endif
{
	struct compat_siginfo new;
	memset(&new, 0, sizeof(new));

	new.si_signo = from->si_signo;
	new.si_errno = from->si_errno;
	new.si_code  = from->si_code;
	switch(siginfo_layout(from->si_signo, from->si_code)) {
	case SIL_KILL:
		new.si_pid = from->si_pid;
		new.si_uid = from->si_uid;
		break;
	case SIL_TIMER:
		new.si_tid     = from->si_tid;
		new.si_overrun = from->si_overrun;
		new.si_int     = from->si_int;
		break;
	case SIL_POLL:
		new.si_band = from->si_band;
		new.si_fd   = from->si_fd;
		break;
	case SIL_FAULT:
		new.si_addr = ptr_to_compat(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		new.si_trapno = from->si_trapno;
#endif
		break;
	case SIL_FAULT_MCEERR:
		new.si_addr = ptr_to_compat(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		new.si_trapno = from->si_trapno;
#endif
		new.si_addr_lsb = from->si_addr_lsb;
		break;
	case SIL_FAULT_BNDERR:
		new.si_addr = ptr_to_compat(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		new.si_trapno = from->si_trapno;
#endif
		new.si_lower = ptr_to_compat(from->si_lower);
		new.si_upper = ptr_to_compat(from->si_upper);
		break;
	case SIL_FAULT_PKUERR:
		new.si_addr = ptr_to_compat(from->si_addr);
#ifdef __ARCH_SI_TRAPNO
		new.si_trapno = from->si_trapno;
#endif
		new.si_pkey = from->si_pkey;
		break;
	case SIL_CHLD:
		new.si_pid    = from->si_pid;
		new.si_uid    = from->si_uid;
		new.si_status = from->si_status;
#ifdef CONFIG_X86_X32_ABI
		if (x32_ABI) {
			new._sifields._sigchld_x32._utime = from->si_utime;
			new._sifields._sigchld_x32._stime = from->si_stime;
		} else
#endif
		{
			new.si_utime = from->si_utime;
			new.si_stime = from->si_stime;
		}
		break;
	case SIL_RT:
		new.si_pid = from->si_pid;
		new.si_uid = from->si_uid;
		new.si_int = from->si_int;
		break;
	case SIL_SYS:
		new.si_call_addr = ptr_to_compat(from->si_call_addr);
		new.si_syscall   = from->si_syscall;
		new.si_arch      = from->si_arch;
		break;
	}

	if (copy_to_user(to, &new, sizeof(struct compat_siginfo)))
		return -EFAULT;

	return 0;
}