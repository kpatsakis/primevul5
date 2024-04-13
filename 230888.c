static void respond_string(const char *p, struct tty_port *port)
{
	while (*p) {
		tty_insert_flip_char(port, *p, 0);
		p++;
	}
	tty_schedule_flip(port);
}