static int __init lp_init (void)
{
	int i, err = 0;

	if (parport_nr[0] == LP_PARPORT_OFF)
		return 0;

	for (i = 0; i < LP_NO; i++) {
		lp_table[i].dev = NULL;
		lp_table[i].flags = 0;
		lp_table[i].chars = LP_INIT_CHAR;
		lp_table[i].time = LP_INIT_TIME;
		lp_table[i].wait = LP_INIT_WAIT;
		lp_table[i].lp_buffer = NULL;
#ifdef LP_STATS
		lp_table[i].lastcall = 0;
		lp_table[i].runchars = 0;
		memset (&lp_table[i].stats, 0, sizeof (struct lp_stats));
#endif
		lp_table[i].last_error = 0;
		init_waitqueue_head (&lp_table[i].waitq);
		init_waitqueue_head (&lp_table[i].dataq);
		mutex_init(&lp_table[i].port_mutex);
		lp_table[i].timeout = 10 * HZ;
	}

	if (register_chrdev (LP_MAJOR, "lp", &lp_fops)) {
		printk (KERN_ERR "lp: unable to get major %d\n", LP_MAJOR);
		return -EIO;
	}

	lp_class = class_create(THIS_MODULE, "printer");
	if (IS_ERR(lp_class)) {
		err = PTR_ERR(lp_class);
		goto out_reg;
	}

	if (parport_register_driver (&lp_driver)) {
		printk (KERN_ERR "lp: unable to register with parport\n");
		err = -EIO;
		goto out_class;
	}

	if (!lp_count) {
		printk (KERN_INFO "lp: driver loaded but no devices found\n");
#ifndef CONFIG_PARPORT_1284
		if (parport_nr[0] == LP_PARPORT_AUTO)
			printk (KERN_INFO "lp: (is IEEE 1284 support enabled?)\n");
#endif
	}

	return 0;

out_class:
	class_destroy(lp_class);
out_reg:
	unregister_chrdev(LP_MAJOR, "lp");
	return err;
}