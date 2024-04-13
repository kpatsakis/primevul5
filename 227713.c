static int __init uvc_init(void)
{
	int ret;

	uvc_debugfs_init();

	ret = usb_register(&uvc_driver.driver);
	if (ret < 0) {
		uvc_debugfs_cleanup();
		return ret;
	}

	printk(KERN_INFO DRIVER_DESC " (" DRIVER_VERSION ")\n");
	return 0;
}