static void update_unchanged_dev_desc(struct usb_device_descriptor *new,
		const struct usb_device_descriptor *old)
{
	__le16 idVendor;
	__le16 idProduct;
	__le16 bcdDevice;
	u8 iSerialNumber;
	u8 iManufacturer;
	u8 iProduct;

	/*
	 * these variables may have been set in
	 * usb_composite_overwrite_options()
	 */
	idVendor = new->idVendor;
	idProduct = new->idProduct;
	bcdDevice = new->bcdDevice;
	iSerialNumber = new->iSerialNumber;
	iManufacturer = new->iManufacturer;
	iProduct = new->iProduct;

	*new = *old;
	if (idVendor)
		new->idVendor = idVendor;
	if (idProduct)
		new->idProduct = idProduct;
	if (bcdDevice)
		new->bcdDevice = bcdDevice;
	else
		new->bcdDevice = cpu_to_le16(get_default_bcdDevice());
	if (iSerialNumber)
		new->iSerialNumber = iSerialNumber;
	if (iManufacturer)
		new->iManufacturer = iManufacturer;
	if (iProduct)
		new->iProduct = iProduct;
}