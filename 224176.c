static inline int is_si_special(const struct kernel_siginfo *info)
{
	return info <= SEND_SIG_PRIV;
}