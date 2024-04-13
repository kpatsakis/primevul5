COMPAT_SYSCALL_DEFINE4(rt_sigtimedwait_time32, compat_sigset_t __user *, uthese,
		struct compat_siginfo __user *, uinfo,
		struct old_timespec32 __user *, uts, compat_size_t, sigsetsize)
{
	sigset_t s;
	struct timespec64 t;
	kernel_siginfo_t info;
	long ret;

	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (get_compat_sigset(&s, uthese))
		return -EFAULT;

	if (uts) {
		if (get_old_timespec32(&t, uts))
			return -EFAULT;
	}

	ret = do_sigtimedwait(&s, &info, uts ? &t : NULL);

	if (ret > 0 && uinfo) {
		if (copy_siginfo_to_user32(uinfo, &info))
			ret = -EFAULT;
	}

	return ret;
}