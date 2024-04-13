static int tiocsetd(struct tty_struct *tty, int __user *p)
{
	int disc;
	int ret;

	if (get_user(disc, p))
		return -EFAULT;

	ret = tty_set_ldisc(tty, disc);

	return ret;
}