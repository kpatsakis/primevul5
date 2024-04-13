SYSCALL_DEFINE4(rt_tgsigqueueinfo, pid_t, tgid, pid_t, pid, int, sig,
		siginfo_t __user *, uinfo)
{
	kernel_siginfo_t info;
	int ret = __copy_siginfo_from_user(sig, &info, uinfo);
	if (unlikely(ret))
		return ret;
	return do_rt_tgsigqueueinfo(tgid, pid, sig, &info);
}