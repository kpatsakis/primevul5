static int tty_tiocgserial(struct tty_struct *tty, struct serial_struct __user *ss)
{
	struct serial_struct v;
	int err;

	memset(&v, 0, sizeof(v));
	if (!tty->ops->get_serial)
		return -ENOTTY;
	err = tty->ops->get_serial(tty, &v);
	if (!err && copy_to_user(ss, &v, sizeof(v)))
		err = -EFAULT;
	return err;
}