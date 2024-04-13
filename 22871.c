void usb_composite_unregister(struct usb_composite_driver *driver)
{
	usb_gadget_unregister_driver(&driver->gadget_driver);
}