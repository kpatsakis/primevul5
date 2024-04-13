static void printer_dev_free(struct kref *kref)
{
	struct printer_dev *dev = container_of(kref, struct printer_dev, kref);

	kfree(dev);
}