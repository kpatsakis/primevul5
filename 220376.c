static struct device *tty_get_device(struct tty_struct *tty)
{
	dev_t devt = tty_devnum(tty);
	return class_find_device_by_devt(tty_class, devt);
}