BOOL msusb_msinterface_replace(MSUSB_CONFIG_DESCRIPTOR* MsConfig, BYTE InterfaceNumber,
                               MSUSB_INTERFACE_DESCRIPTOR* NewMsInterface)
{
	if (!MsConfig || !MsConfig->MsInterfaces)
		return FALSE;

	msusb_msinterface_free(MsConfig->MsInterfaces[InterfaceNumber]);
	MsConfig->MsInterfaces[InterfaceNumber] = NewMsInterface;
	return TRUE;
}