__group_send_sig_info(int sig, struct kernel_siginfo *info, struct task_struct *p)
{
	return send_signal(sig, info, p, PIDTYPE_TGID);
}