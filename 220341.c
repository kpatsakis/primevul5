static int tiocgetd(struct tty_struct *tty, int __user *p)
{
	struct tty_ldisc *ld;
	int ret;

	ld = tty_ldisc_ref_wait(tty);
	if (!ld)
		return -EIO;
	ret = put_user(ld->ops->num, p);
	tty_ldisc_deref(ld);
	return ret;
}