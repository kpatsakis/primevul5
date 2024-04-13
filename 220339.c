void tty_vhangup(struct tty_struct *tty)
{
	tty_debug_hangup(tty, "vhangup\n");
	__tty_hangup(tty, 0);
}