static MSUSB_INTERFACE_DESCRIPTOR** msusb_msinterface_read_list(wStream* s, UINT32 NumInterfaces)
{
	UINT32 inum;
	MSUSB_INTERFACE_DESCRIPTOR** MsInterfaces;
	MsInterfaces =
	    (MSUSB_INTERFACE_DESCRIPTOR**)calloc(NumInterfaces, sizeof(MSUSB_INTERFACE_DESCRIPTOR*));

	if (!MsInterfaces)
		return NULL;

	for (inum = 0; inum < NumInterfaces; inum++)
	{
		MsInterfaces[inum] = msusb_msinterface_read(s);

		if (!MsInterfaces[inum])
			goto fail;
	}

	return MsInterfaces;
fail:

	for (inum = 0; inum < NumInterfaces; inum++)
		msusb_msinterface_free(MsInterfaces[inum]);

	free(MsInterfaces);
	return NULL;
}