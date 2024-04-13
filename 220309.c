static long tty_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	struct tty_struct *tty = file_tty(file);
	struct tty_ldisc *ld;
	int retval = -ENOIOCTLCMD;

	switch (cmd) {
	case TIOCOUTQ:
	case TIOCSTI:
	case TIOCGWINSZ:
	case TIOCSWINSZ:
	case TIOCGEXCL:
	case TIOCGETD:
	case TIOCSETD:
	case TIOCGDEV:
	case TIOCMGET:
	case TIOCMSET:
	case TIOCMBIC:
	case TIOCMBIS:
	case TIOCGICOUNT:
	case TIOCGPGRP:
	case TIOCSPGRP:
	case TIOCGSID:
	case TIOCSERGETLSR:
	case TIOCGRS485:
	case TIOCSRS485:
#ifdef TIOCGETP
	case TIOCGETP:
	case TIOCSETP:
	case TIOCSETN:
#endif
#ifdef TIOCGETC
	case TIOCGETC:
	case TIOCSETC:
#endif
#ifdef TIOCGLTC
	case TIOCGLTC:
	case TIOCSLTC:
#endif
	case TCSETSF:
	case TCSETSW:
	case TCSETS:
	case TCGETS:
#ifdef TCGETS2
	case TCGETS2:
	case TCSETSF2:
	case TCSETSW2:
	case TCSETS2:
#endif
	case TCGETA:
	case TCSETAF:
	case TCSETAW:
	case TCSETA:
	case TIOCGLCKTRMIOS:
	case TIOCSLCKTRMIOS:
#ifdef TCGETX
	case TCGETX:
	case TCSETX:
	case TCSETXW:
	case TCSETXF:
#endif
	case TIOCGSOFTCAR:
	case TIOCSSOFTCAR:

	case PPPIOCGCHAN:
	case PPPIOCGUNIT:
		return tty_ioctl(file, cmd, (unsigned long)compat_ptr(arg));
	case TIOCCONS:
	case TIOCEXCL:
	case TIOCNXCL:
	case TIOCVHANGUP:
	case TIOCSBRK:
	case TIOCCBRK:
	case TCSBRK:
	case TCSBRKP:
	case TCFLSH:
	case TIOCGPTPEER:
	case TIOCNOTTY:
	case TIOCSCTTY:
	case TCXONC:
	case TIOCMIWAIT:
	case TIOCSERCONFIG:
		return tty_ioctl(file, cmd, arg);
	}

	if (tty_paranoia_check(tty, file_inode(file), "tty_ioctl"))
		return -EINVAL;

	switch (cmd) {
	case TIOCSSERIAL:
		return compat_tty_tiocsserial(tty, compat_ptr(arg));
	case TIOCGSERIAL:
		return compat_tty_tiocgserial(tty, compat_ptr(arg));
	}
	if (tty->ops->compat_ioctl) {
		retval = tty->ops->compat_ioctl(tty, cmd, arg);
		if (retval != -ENOIOCTLCMD)
			return retval;
	}

	ld = tty_ldisc_ref_wait(tty);
	if (!ld)
		return hung_up_tty_compat_ioctl(file, cmd, arg);
	if (ld->ops->compat_ioctl)
		retval = ld->ops->compat_ioctl(tty, file, cmd, arg);
	if (retval == -ENOIOCTLCMD && ld->ops->ioctl)
		retval = ld->ops->ioctl(tty, file,
				(unsigned long)compat_ptr(cmd), arg);
	tty_ldisc_deref(ld);

	return retval;
}