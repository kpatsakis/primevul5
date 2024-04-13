int force_sig_info(struct kernel_siginfo *info)
{
	return force_sig_info_to_task(info, current);
}