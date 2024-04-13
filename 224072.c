static inline void prepare_kill_siginfo(int sig, struct kernel_siginfo *info)
{
	clear_siginfo(info);
	info->si_signo = sig;
	info->si_errno = 0;
	info->si_code = SI_USER;
	info->si_pid = task_tgid_vnr(current);
	info->si_uid = from_kuid_munged(current_user_ns(), current_uid());
}