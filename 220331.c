void tty_vhangup_self(void)
{
	struct tty_struct *tty;

	tty = get_current_tty();
	if (tty) {
		tty_vhangup(tty);
		tty_kref_put(tty);
	}
}