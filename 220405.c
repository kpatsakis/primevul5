void tty_kclose(struct tty_struct *tty)
{
	/*
	 * Ask the line discipline code to release its structures
	 */
	tty_ldisc_release(tty);

	/* Wait for pending work before tty destruction commmences */
	tty_flush_works(tty);

	tty_debug_hangup(tty, "freeing structure\n");
	/*
	 * The release_tty function takes care of the details of clearing
	 * the slots and preserving the termios structure.
	 */
	mutex_lock(&tty_mutex);
	tty_port_set_kopened(tty->port, 0);
	release_tty(tty, tty->index);
	mutex_unlock(&tty_mutex);
}