void tty_vhangup_session(struct tty_struct *tty)
{
	tty_debug_hangup(tty, "session hangup\n");
	__tty_hangup(tty, 1);
}