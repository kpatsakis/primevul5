static int tty_tiocmset(struct tty_struct *tty, unsigned int cmd,
	     unsigned __user *p)
{
	int retval;
	unsigned int set, clear, val;

	if (tty->ops->tiocmset == NULL)
		return -EINVAL;

	retval = get_user(val, p);
	if (retval)
		return retval;
	set = clear = 0;
	switch (cmd) {
	case TIOCMBIS:
		set = val;
		break;
	case TIOCMBIC:
		clear = val;
		break;
	case TIOCMSET:
		set = val;
		clear = ~val;
		break;
	}
	set &= TIOCM_DTR|TIOCM_RTS|TIOCM_OUT1|TIOCM_OUT2|TIOCM_LOOP;
	clear &= TIOCM_DTR|TIOCM_RTS|TIOCM_OUT1|TIOCM_OUT2|TIOCM_LOOP;
	return tty->ops->tiocmset(tty, set, clear);
}