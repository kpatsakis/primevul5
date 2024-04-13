static int lp_register(int nr, struct parport *port)
{
	lp_table[nr].dev = parport_register_device(port, "lp", 
						   lp_preempt, NULL, NULL, 0,
						   (void *) &lp_table[nr]);
	if (lp_table[nr].dev == NULL)
		return 1;
	lp_table[nr].flags |= LP_EXIST;

	if (reset)
		lp_reset(nr);

	device_create(lp_class, port->dev, MKDEV(LP_MAJOR, nr), NULL,
		      "lp%d", nr);

	printk(KERN_INFO "lp%d: using %s (%s).\n", nr, port->name, 
	       (port->irq == PARPORT_IRQ_NONE)?"polling":"interrupt-driven");

#ifdef CONFIG_LP_CONSOLE
	if (!nr) {
		if (port->modes & PARPORT_MODE_SAFEININT) {
			register_console(&lpcons);
			console_registered = port;
			printk (KERN_INFO "lp%d: console ready\n", CONSOLE_LP);
		} else
			printk (KERN_ERR "lp%d: cannot run console on %s\n",
				CONSOLE_LP, port->name);
	}
#endif

	return 0;
}