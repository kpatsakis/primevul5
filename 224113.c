static int do_rt_sigqueueinfo(pid_t pid, int sig, kernel_siginfo_t *info)
{
	/* Not even root can pretend to send signals from the kernel.
	 * Nor can they impersonate a kill()/tgkill(), which adds source info.
	 */
	if ((info->si_code >= 0 || info->si_code == SI_TKILL) &&
	    (task_pid_vnr(current) != pid))
		return -EPERM;

	/* POSIX.1b doesn't mention process groups.  */
	return kill_proc_info(sig, info, pid);
}