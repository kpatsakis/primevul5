static inline bool vt_in_use(unsigned int i)
{
	extern struct tty_driver *console_driver;

	return console_driver->ttys[i] && console_driver->ttys[i]->count;
}