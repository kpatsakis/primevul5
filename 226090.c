static void gprinter_cleanup(void)
{
	if (major) {
		unregister_chrdev_region(MKDEV(major, 0), minors);
		major = minors = 0;
	}
	class_destroy(usb_gadget_class);
	usb_gadget_class = NULL;
}