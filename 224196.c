int copy_siginfo_to_user32(struct compat_siginfo __user *to,
			   const struct kernel_siginfo *from)
#if defined(CONFIG_X86_X32_ABI) || defined(CONFIG_IA32_EMULATION)
{
	return __copy_siginfo_to_user32(to, from, in_x32_syscall());
}