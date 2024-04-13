static inline void status_report(struct tty_struct *tty)
{
	respond_string("\033[0n", tty->port);	/* Terminal ok */
}