static int compat_tty_tiocgserial(struct tty_struct *tty,
			struct serial_struct32 __user *ss)
{
	struct serial_struct32 v32;
	struct serial_struct v;
	int err;

	memset(&v, 0, sizeof(v));
	memset(&v32, 0, sizeof(v32));

	if (!tty->ops->get_serial)
		return -ENOTTY;
	err = tty->ops->get_serial(tty, &v);
	if (!err) {
		memcpy(&v32, &v, offsetof(struct serial_struct32, iomem_base));
		v32.iomem_base = (unsigned long)v.iomem_base >> 32 ?
			0xfffffff : ptr_to_compat(v.iomem_base);
		v32.iomem_reg_shift = v.iomem_reg_shift;
		v32.port_high = v.port_high;
		if (copy_to_user(ss, &v32, sizeof(v32)))
			err = -EFAULT;
	}
	return err;
}