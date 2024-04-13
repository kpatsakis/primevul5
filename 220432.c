static int tiocsti(struct tty_struct *tty, char __user *p)
{
	char ch, mbz = 0;
	struct tty_ldisc *ld;

	if ((current->signal->tty != tty) && !capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (get_user(ch, p))
		return -EFAULT;
	tty_audit_tiocsti(tty, ch);
	ld = tty_ldisc_ref_wait(tty);
	if (!ld)
		return -EIO;
	if (ld->ops->receive_buf)
		ld->ops->receive_buf(tty, &ch, &mbz, 1);
	tty_ldisc_deref(ld);
	return 0;
}