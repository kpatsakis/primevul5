long tty_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct tty_struct *tty = file_tty(file);
	struct tty_struct *real_tty;
	void __user *p = (void __user *)arg;
	int retval;
	struct tty_ldisc *ld;

	if (tty_paranoia_check(tty, file_inode(file), "tty_ioctl"))
		return -EINVAL;

	real_tty = tty_pair_get_tty(tty);

	/*
	 * Factor out some common prep work
	 */
	switch (cmd) {
	case TIOCSETD:
	case TIOCSBRK:
	case TIOCCBRK:
	case TCSBRK:
	case TCSBRKP:
		retval = tty_check_change(tty);
		if (retval)
			return retval;
		if (cmd != TIOCCBRK) {
			tty_wait_until_sent(tty, 0);
			if (signal_pending(current))
				return -EINTR;
		}
		break;
	}

	/*
	 *	Now do the stuff.
	 */
	switch (cmd) {
	case TIOCSTI:
		return tiocsti(tty, p);
	case TIOCGWINSZ:
		return tiocgwinsz(real_tty, p);
	case TIOCSWINSZ:
		return tiocswinsz(real_tty, p);
	case TIOCCONS:
		return real_tty != tty ? -EINVAL : tioccons(file);
	case FIONBIO:
		return fionbio(file, p);
	case TIOCEXCL:
		set_bit(TTY_EXCLUSIVE, &tty->flags);
		return 0;
	case TIOCNXCL:
		clear_bit(TTY_EXCLUSIVE, &tty->flags);
		return 0;
	case TIOCGEXCL:
	{
		int excl = test_bit(TTY_EXCLUSIVE, &tty->flags);
		return put_user(excl, (int __user *)p);
	}
	case TIOCNOTTY:
		if (current->signal->tty != tty)
			return -ENOTTY;
		no_tty();
		return 0;
	case TIOCSCTTY:
		return tiocsctty(real_tty, file, arg);
	case TIOCGPGRP:
		return tiocgpgrp(tty, real_tty, p);
	case TIOCSPGRP:
		return tiocspgrp(tty, real_tty, p);
 	case TIOCGSID:
 		return tiocgsid(tty, real_tty, p);
 	case TIOCGETD:
		return put_user(tty->ldisc->ops->num, (int __user *)p);
 	case TIOCSETD:
 		return tiocsetd(tty, p);
 	case TIOCVHANGUP:
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		tty_vhangup(tty);
		return 0;
	case TIOCGDEV:
	{
		unsigned int ret = new_encode_dev(tty_devnum(real_tty));
		return put_user(ret, (unsigned int __user *)p);
	}
	/*
	 * Break handling
	 */
	case TIOCSBRK:	/* Turn break on, unconditionally */
		if (tty->ops->break_ctl)
			return tty->ops->break_ctl(tty, -1);
		return 0;
	case TIOCCBRK:	/* Turn break off, unconditionally */
		if (tty->ops->break_ctl)
			return tty->ops->break_ctl(tty, 0);
		return 0;
	case TCSBRK:   /* SVID version: non-zero arg --> no break */
		/* non-zero arg means wait for all output data
		 * to be sent (performed above) but don't send break.
		 * This is used by the tcdrain() termios function.
		 */
		if (!arg)
			return send_break(tty, 250);
		return 0;
	case TCSBRKP:	/* support for POSIX tcsendbreak() */
		return send_break(tty, arg ? arg*100 : 250);

	case TIOCMGET:
		return tty_tiocmget(tty, p);
	case TIOCMSET:
	case TIOCMBIC:
	case TIOCMBIS:
		return tty_tiocmset(tty, cmd, p);
	case TIOCGICOUNT:
		retval = tty_tiocgicount(tty, p);
		/* For the moment allow fall through to the old method */
        	if (retval != -EINVAL)
			return retval;
		break;
	case TCFLSH:
		switch (arg) {
		case TCIFLUSH:
		case TCIOFLUSH:
		/* flush tty buffer and allow ldisc to process ioctl */
			tty_buffer_flush(tty, NULL);
			break;
		}
		break;
	case TIOCSSERIAL:
		tty_warn_deprecated_flags(p);
		break;
	}
	if (tty->ops->ioctl) {
		retval = tty->ops->ioctl(tty, cmd, arg);
		if (retval != -ENOIOCTLCMD)
			return retval;
	}
	ld = tty_ldisc_ref_wait(tty);
	retval = -EINVAL;
	if (ld->ops->ioctl) {
		retval = ld->ops->ioctl(tty, file, cmd, arg);
		if (retval == -ENOIOCTLCMD)
			retval = -ENOTTY;
	}
	tty_ldisc_deref(ld);
	return retval;
}
