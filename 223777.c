static void msusb_msinterface_free_list(MSUSB_INTERFACE_DESCRIPTOR** MsInterfaces,
                                        UINT32 NumInterfaces)
{
	UINT32 inum = 0;

	if (MsInterfaces)
	{
		for (inum = 0; inum < NumInterfaces; inum++)
		{
			msusb_msinterface_free(MsInterfaces[inum]);
		}

		free(MsInterfaces);
	}
}