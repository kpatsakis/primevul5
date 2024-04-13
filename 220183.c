extern void x11_get_display(uint16_t *port, char **target)
{
	char *display, *port_split, *port_period;
	*target = NULL;

	display = xstrdup(getenv("DISPLAY"));

	if (!display) {
		error("No DISPLAY variable set, cannot setup x11 forwarding.");
		exit(-1);
	}

	if (display[0] == ':') {
		struct stat st;
		char *screen_period;
		*port = 0;
		screen_period = strchr(display, '.');
		if (screen_period)
			*screen_period = '\0';
		xstrfmtcat(*target, "/tmp/.X11-unix/X%s", display + 1);
		xfree(display);
		if (stat(*target, &st) != 0) {
			error("Cannot stat() local X11 socket `%s`", *target);
			exit(-1);
		}
		return;
	}

	/*
	 * Parse out port number
	 * Example: localhost/unix:89.0 or localhost/unix:89
	 */
	port_split = strchr(display, ':');
	if (!port_split) {
		error("Error parsing DISPLAY environment variable. "
		      "Cannot use X11 forwarding.");
		exit(-1);
	}
	*port_split = '\0';

	/*
	 * Handle the "screen" portion of the display port.
	 * Xorg does not require a screen to be specified, defaults to 0.
	 */
	port_split++;
	port_period = strchr(port_split, '.');
	if (port_period)
		*port_period = '\0';

	*port = atoi(port_split) + X11_TCP_PORT_OFFSET;
	*target = display;
}