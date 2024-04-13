static MSUSB_INTERFACE_DESCRIPTOR* msusb_msinterface_new()
{
	return (MSUSB_INTERFACE_DESCRIPTOR*)calloc(1, sizeof(MSUSB_INTERFACE_DESCRIPTOR));
}