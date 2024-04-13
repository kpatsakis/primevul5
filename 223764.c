BOOL msusb_msconfig_write(MSUSB_CONFIG_DESCRIPTOR* MsConfg, wStream* out)
{
	UINT32 inum = 0;
	MSUSB_INTERFACE_DESCRIPTOR** MsInterfaces;
	MSUSB_INTERFACE_DESCRIPTOR* MsInterface;

	if (!MsConfg)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(out, 8))
		return FALSE;

	/* ConfigurationHandle*/
	Stream_Write_UINT32(out, MsConfg->ConfigurationHandle);
	/* NumInterfaces*/
	Stream_Write_UINT32(out, MsConfg->NumInterfaces);
	/* Interfaces */
	MsInterfaces = MsConfg->MsInterfaces;

	for (inum = 0; inum < MsConfg->NumInterfaces; inum++)
	{
		MsInterface = MsInterfaces[inum];

		if (!msusb_msinterface_write(MsInterface, out))
			return FALSE;
	}

	return TRUE;
}