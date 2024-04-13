static int compat_tty_tiocsserial(struct tty_struct *tty,
		struct serial_struct32 __user *ss)
{
	static DEFINE_RATELIMIT_STATE(depr_flags,
			DEFAULT_RATELIMIT_INTERVAL,
			DEFAULT_RATELIMIT_BURST);
	char comm[TASK_COMM_LEN];
	struct serial_struct32 v32;
	struct serial_struct v;
	int flags;

	if (copy_from_user(&v32, ss, sizeof(*ss)))
		return -EFAULT;

	memcpy(&v, &v32, offsetof(struct serial_struct32, iomem_base));
	v.iomem_base = compat_ptr(v32.iomem_base);
	v.iomem_reg_shift = v32.iomem_reg_shift;
	v.port_high = v32.port_high;
	v.iomap_base = 0;

	flags = v.flags & ASYNC_DEPRECATED;

	if (flags && __ratelimit(&depr_flags))
		pr_warn("%s: '%s' is using deprecated serial flags (with no effect): %.8x\n",
			__func__, get_task_comm(comm, current), flags);
	if (!tty->ops->set_serial)
		return -ENOTTY;
	return tty->ops->set_serial(tty, &v);
}