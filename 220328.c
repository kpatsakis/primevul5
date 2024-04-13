static void release_tty(struct tty_struct *tty, int idx)
{
	/* This should always be true but check for the moment */
	WARN_ON(tty->index != idx);
	WARN_ON(!mutex_is_locked(&tty_mutex));
	if (tty->ops->shutdown)
		tty->ops->shutdown(tty);
	tty_save_termios(tty);
	tty_driver_remove_tty(tty->driver, tty);
	if (tty->port)
		tty->port->itty = NULL;
	if (tty->link)
		tty->link->port->itty = NULL;
	if (tty->port)
		tty_buffer_cancel_work(tty->port);
	if (tty->link)
		tty_buffer_cancel_work(tty->link->port);

	tty_kref_put(tty->link);
	tty_kref_put(tty);
}