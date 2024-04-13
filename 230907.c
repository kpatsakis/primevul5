static inline void respond_ID(struct tty_struct *tty)
{
	respond_string(VT102ID, tty->port);
}