static struct tty_struct *tty_driver_lookup_tty(struct tty_driver *driver,
		struct file *file, int idx)
{
	struct tty_struct *tty;

	if (driver->ops->lookup)
		if (!file)
			tty = ERR_PTR(-EIO);
		else
			tty = driver->ops->lookup(driver, file, idx);
	else
		tty = driver->ttys[idx];

	if (!IS_ERR(tty))
		tty_kref_get(tty);
	return tty;
}