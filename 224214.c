COMPAT_SYSCALL_DEFINE4(rt_tgsigqueueinfo,
			compat_pid_t, tgid,
			compat_pid_t, pid,
			int, sig,
			struct compat_siginfo __user *, uinfo)
{
	kernel_siginfo_t info;
	int ret = __copy_siginfo_from_user32(sig, &info, uinfo);
	if (unlikely(ret))
		return ret;
	return do_rt_tgsigqueueinfo(tgid, pid, sig, &info);
}