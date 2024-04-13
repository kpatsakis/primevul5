MSUSB_CONFIG_DESCRIPTOR* msusb_msconfig_new(void)
{
	return (MSUSB_CONFIG_DESCRIPTOR*)calloc(1, sizeof(MSUSB_CONFIG_DESCRIPTOR));
}