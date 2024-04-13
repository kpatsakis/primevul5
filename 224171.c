COMPAT_SYSCALL_DEFINE3(sigaction, int, sig,
		const struct compat_old_sigaction __user *, act,
	        struct compat_old_sigaction __user *, oact)
{
	struct k_sigaction new_ka, old_ka;
	int ret;
	compat_old_sigset_t mask;
	compat_uptr_t handler, restorer;

	if (act) {
		if (!access_ok(act, sizeof(*act)) ||
		    __get_user(handler, &act->sa_handler) ||
		    __get_user(restorer, &act->sa_restorer) ||
		    __get_user(new_ka.sa.sa_flags, &act->sa_flags) ||
		    __get_user(mask, &act->sa_mask))
			return -EFAULT;

#ifdef __ARCH_HAS_KA_RESTORER
		new_ka.ka_restorer = NULL;
#endif
		new_ka.sa.sa_handler = compat_ptr(handler);
		new_ka.sa.sa_restorer = compat_ptr(restorer);
		siginitset(&new_ka.sa.sa_mask, mask);
	}

	ret = do_sigaction(sig, act ? &new_ka : NULL, oact ? &old_ka : NULL);

	if (!ret && oact) {
		if (!access_ok(oact, sizeof(*oact)) ||
		    __put_user(ptr_to_compat(old_ka.sa.sa_handler),
			       &oact->sa_handler) ||
		    __put_user(ptr_to_compat(old_ka.sa.sa_restorer),
			       &oact->sa_restorer) ||
		    __put_user(old_ka.sa.sa_flags, &oact->sa_flags) ||
		    __put_user(old_ka.sa.sa_mask.sig[0], &oact->sa_mask))
			return -EFAULT;
	}
	return ret;
}