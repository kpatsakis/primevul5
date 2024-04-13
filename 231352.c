__init static int clear_boot_tracer(void)
{
	/*
	 * The default tracer at boot buffer is an init section.
	 * This function is called in lateinit. If we did not
	 * find the boot tracer, then clear it out, to prevent
	 * later registration from accessing the buffer that is
	 * about to be freed.
	 */
	if (!default_bootup_tracer)
		return 0;

	printk(KERN_INFO "ftrace bootup tracer '%s' not registered.\n",
	       default_bootup_tracer);
	default_bootup_tracer = NULL;

	return 0;
}