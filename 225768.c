int fork_usermode_driver(struct umd_info *info)
{
	struct subprocess_info *sub_info;
	const char *argv[] = { info->driver_name, NULL };
	int err;

	if (WARN_ON_ONCE(info->tgid))
		return -EBUSY;

	err = -ENOMEM;
	sub_info = call_usermodehelper_setup(info->driver_name,
					     (char **)argv, NULL, GFP_KERNEL,
					     umd_setup, umd_cleanup, info);
	if (!sub_info)
		goto out;

	err = call_usermodehelper_exec(sub_info, UMH_WAIT_EXEC);
out:
	return err;
}