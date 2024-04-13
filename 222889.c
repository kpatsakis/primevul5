sudoers_policy_register_hooks(int version, int (*register_hook)(struct sudo_hook *hook))
{
    struct sudo_hook *hook;

    for (hook = sudoers_hooks; hook->hook_fn != NULL; hook++) {
	if (register_hook(hook) != 0) {
	    sudo_warn_nodebug(
		U_("unable to register hook of type %d (version %d.%d)"),
		hook->hook_type, SUDO_API_VERSION_GET_MAJOR(hook->hook_version),
		SUDO_API_VERSION_GET_MINOR(hook->hook_version));
	}
    }
}