static void lp_attach (struct parport *port)
{
	unsigned int i;

	switch (parport_nr[0]) {
	case LP_PARPORT_UNSPEC:
	case LP_PARPORT_AUTO:
		if (parport_nr[0] == LP_PARPORT_AUTO &&
		    port->probe_info[0].class != PARPORT_CLASS_PRINTER)
			return;
		if (lp_count == LP_NO) {
			printk(KERN_INFO "lp: ignoring parallel port (max. %d)\n",LP_NO);
			return;
		}
		if (!lp_register(lp_count, port))
			lp_count++;
		break;

	default:
		for (i = 0; i < LP_NO; i++) {
			if (port->number == parport_nr[i]) {
				if (!lp_register(i, port))
					lp_count++;
				break;
			}
		}
		break;
	}
}