struct tty_struct *tty_kopen(dev_t device)
{
	struct tty_struct *tty;
	struct tty_driver *driver;
	int index = -1;

	mutex_lock(&tty_mutex);
	driver = tty_lookup_driver(device, NULL, &index);
	if (IS_ERR(driver)) {
		mutex_unlock(&tty_mutex);
		return ERR_CAST(driver);
	}

	/* check whether we're reopening an existing tty */
	tty = tty_driver_lookup_tty(driver, NULL, index);
	if (IS_ERR(tty))
		goto out;

	if (tty) {
		/* drop kref from tty_driver_lookup_tty() */
		tty_kref_put(tty);
		tty = ERR_PTR(-EBUSY);
	} else { /* tty_init_dev returns tty with the tty_lock held */
		tty = tty_init_dev(driver, index);
		if (IS_ERR(tty))
			goto out;
		tty_port_set_kopened(tty->port, 1);
	}
out:
	mutex_unlock(&tty_mutex);
	tty_driver_kref_put(driver);
	return tty;
}