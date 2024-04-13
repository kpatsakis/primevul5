SYSCALL_DEFINE4(rt_sigtimedwait_time32, const sigset_t __user *, uthese,
		siginfo_t __user *, uinfo,
		const struct old_timespec32 __user *, uts,
		size_t, sigsetsize)
{
	sigset_t these;
	struct timespec64 ts;
	kernel_siginfo_t info;
	int ret;

	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&these, uthese, sizeof(these)))
		return -EFAULT;

	if (uts) {
		if (get_old_timespec32(&ts, uts))
			return -EFAULT;
	}

	ret = do_sigtimedwait(&these, &info, uts ? &ts : NULL);

	if (ret > 0 && uinfo) {
		if (copy_siginfo_to_user(uinfo, &info))
			ret = -EFAULT;
	}

	return ret;
}