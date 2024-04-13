void usb_composite_overwrite_options(struct usb_composite_dev *cdev,
		struct usb_composite_overwrite *covr)
{
	struct usb_device_descriptor	*desc = &cdev->desc;
	struct usb_gadget_strings	*gstr = cdev->driver->strings[0];
	struct usb_string		*dev_str = gstr->strings;

	if (covr->idVendor)
		desc->idVendor = cpu_to_le16(covr->idVendor);

	if (covr->idProduct)
		desc->idProduct = cpu_to_le16(covr->idProduct);

	if (covr->bcdDevice)
		desc->bcdDevice = cpu_to_le16(covr->bcdDevice);

	if (covr->serial_number) {
		desc->iSerialNumber = dev_str[USB_GADGET_SERIAL_IDX].id;
		dev_str[USB_GADGET_SERIAL_IDX].s = covr->serial_number;
	}
	if (covr->manufacturer) {
		desc->iManufacturer = dev_str[USB_GADGET_MANUFACTURER_IDX].id;
		dev_str[USB_GADGET_MANUFACTURER_IDX].s = covr->manufacturer;

	} else if (!strlen(dev_str[USB_GADGET_MANUFACTURER_IDX].s)) {
		desc->iManufacturer = dev_str[USB_GADGET_MANUFACTURER_IDX].id;
		cdev->def_manufacturer = composite_default_mfr(cdev->gadget);
		dev_str[USB_GADGET_MANUFACTURER_IDX].s = cdev->def_manufacturer;
	}

	if (covr->product) {
		desc->iProduct = dev_str[USB_GADGET_PRODUCT_IDX].id;
		dev_str[USB_GADGET_PRODUCT_IDX].s = covr->product;
	}
}