static void lp_detach (struct parport *port)
{
	/* Write this some day. */
#ifdef CONFIG_LP_CONSOLE
	if (console_registered == port) {
		unregister_console(&lpcons);
		console_registered = NULL;
	}
#endif /* CONFIG_LP_CONSOLE */
}