MSUSB_INTERFACE_DESCRIPTOR* msusb_msinterface_read(wStream* s)
{
	MSUSB_INTERFACE_DESCRIPTOR* MsInterface;

	if (Stream_GetRemainingCapacity(s) < 12)
		return NULL;

	MsInterface = msusb_msinterface_new();

	if (!MsInterface)
		return NULL;

	Stream_Read_UINT16(s, MsInterface->Length);
	Stream_Read_UINT16(s, MsInterface->NumberOfPipesExpected);
	Stream_Read_UINT8(s, MsInterface->InterfaceNumber);
	Stream_Read_UINT8(s, MsInterface->AlternateSetting);
	Stream_Seek(s, 2);
	Stream_Read_UINT32(s, MsInterface->NumberOfPipes);
	MsInterface->InterfaceHandle = 0;
	MsInterface->bInterfaceClass = 0;
	MsInterface->bInterfaceSubClass = 0;
	MsInterface->bInterfaceProtocol = 0;
	MsInterface->InitCompleted = 0;
	MsInterface->MsPipes = NULL;

	if (MsInterface->NumberOfPipes > 0)
	{
		MsInterface->MsPipes = msusb_mspipes_read(s, MsInterface->NumberOfPipes);

		if (!MsInterface->MsPipes)
			goto out_error;
	}

	return MsInterface;
out_error:
	msusb_msinterface_free(MsInterface);
	return NULL;
}