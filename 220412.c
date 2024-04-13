static int tty_tiocsserial(struct tty_struct *tty, struct serial_struct __user *ss)
{
	static DEFINE_RATELIMIT_STATE(depr_flags,
			DEFAULT_RATELIMIT_INTERVAL,
			DEFAULT_RATELIMIT_BURST);
	char comm[TASK_COMM_LEN];
	struct serial_struct v;
	int flags;

	if (copy_from_user(&v, ss, sizeof(*ss)))
		return -EFAULT;

	flags = v.flags & ASYNC_DEPRECATED;

	if (flags && __ratelimit(&depr_flags))
		pr_warn("%s: '%s' is using deprecated serial flags (with no effect): %.8x\n",
			__func__, get_task_comm(comm, current), flags);
	if (!tty->ops->set_serial)
		return -ENOTTY;
	return tty->ops->set_serial(tty, &v);
}