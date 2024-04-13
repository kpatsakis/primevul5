static void lp_cleanup_module (void)
{
	unsigned int offset;

	parport_unregister_driver (&lp_driver);

#ifdef CONFIG_LP_CONSOLE
	unregister_console (&lpcons);
#endif

	unregister_chrdev(LP_MAJOR, "lp");
	for (offset = 0; offset < LP_NO; offset++) {
		if (lp_table[offset].dev == NULL)
			continue;
		parport_unregister_device(lp_table[offset].dev);
		device_destroy(lp_class, MKDEV(LP_MAJOR, offset));
	}
	class_destroy(lp_class);
}