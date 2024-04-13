static int check_tty_count(struct tty_struct *tty, const char *routine)
{
#ifdef CHECK_TTY_COUNT
	struct list_head *p;
	int count = 0, kopen_count = 0;

	spin_lock(&tty->files_lock);
	list_for_each(p, &tty->tty_files) {
		count++;
	}
	spin_unlock(&tty->files_lock);
	if (tty->driver->type == TTY_DRIVER_TYPE_PTY &&
	    tty->driver->subtype == PTY_TYPE_SLAVE &&
	    tty->link && tty->link->count)
		count++;
	if (tty_port_kopened(tty->port))
		kopen_count++;
	if (tty->count != (count + kopen_count)) {
		tty_warn(tty, "%s: tty->count(%d) != (#fd's(%d) + #kopen's(%d))\n",
			 routine, tty->count, count, kopen_count);
		return (count + kopen_count);
	}
#endif
	return 0;
}