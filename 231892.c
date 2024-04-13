static int lp_negotiate(struct parport * port, int mode)
{
	if (parport_negotiate (port, mode) != 0) {
		mode = IEEE1284_MODE_COMPAT;
		parport_negotiate (port, mode);
	}

	return (mode);
}