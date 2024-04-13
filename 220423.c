static int tty_reopen(struct tty_struct *tty)
{
	struct tty_driver *driver = tty->driver;
	struct tty_ldisc *ld;
	int retval = 0;

	if (driver->type == TTY_DRIVER_TYPE_PTY &&
	    driver->subtype == PTY_TYPE_MASTER)
		return -EIO;

	if (!tty->count)
		return -EAGAIN;

	if (test_bit(TTY_EXCLUSIVE, &tty->flags) && !capable(CAP_SYS_ADMIN))
		return -EBUSY;

	ld = tty_ldisc_ref_wait(tty);
	if (ld) {
		tty_ldisc_deref(ld);
	} else {
		retval = tty_ldisc_lock(tty, 5 * HZ);
		if (retval)
			return retval;

		if (!tty->ldisc)
			retval = tty_ldisc_reinit(tty, tty->termios.c_line);
		tty_ldisc_unlock(tty);
	}

	if (retval == 0)
		tty->count++;

	return retval;
}