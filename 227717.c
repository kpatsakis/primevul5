static void __exit uvc_cleanup(void)
{
	usb_deregister(&uvc_driver.driver);
	uvc_debugfs_cleanup();
}