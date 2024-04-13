BOOL msusb_msinterface_write(MSUSB_INTERFACE_DESCRIPTOR* MsInterface, wStream* out)
{
	MSUSB_PIPE_DESCRIPTOR** MsPipes;
	MSUSB_PIPE_DESCRIPTOR* MsPipe;
	UINT32 pnum = 0;

	if (!MsInterface)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(out, 16 + MsInterface->NumberOfPipes * 20))
		return FALSE;

	/* Length */
	Stream_Write_UINT16(out, MsInterface->Length);
	/* InterfaceNumber */
	Stream_Write_UINT8(out, MsInterface->InterfaceNumber);
	/* AlternateSetting */
	Stream_Write_UINT8(out, MsInterface->AlternateSetting);
	/* bInterfaceClass */
	Stream_Write_UINT8(out, MsInterface->bInterfaceClass);
	/* bInterfaceSubClass */
	Stream_Write_UINT8(out, MsInterface->bInterfaceSubClass);
	/* bInterfaceProtocol */
	Stream_Write_UINT8(out, MsInterface->bInterfaceProtocol);
	/* Padding */
	Stream_Write_UINT8(out, 0);
	/* InterfaceHandle */
	Stream_Write_UINT32(out, MsInterface->InterfaceHandle);
	/* NumberOfPipes */
	Stream_Write_UINT32(out, MsInterface->NumberOfPipes);
	/* Pipes */
	MsPipes = MsInterface->MsPipes;

	for (pnum = 0; pnum < MsInterface->NumberOfPipes; pnum++)
	{
		MsPipe = MsPipes[pnum];
		/* MaximumPacketSize */
		Stream_Write_UINT16(out, MsPipe->MaximumPacketSize);
		/* EndpointAddress */
		Stream_Write_UINT8(out, MsPipe->bEndpointAddress);
		/* Interval */
		Stream_Write_UINT8(out, MsPipe->bInterval);
		/* PipeType */
		Stream_Write_UINT32(out, MsPipe->PipeType);
		/* PipeHandle */
		Stream_Write_UINT32(out, MsPipe->PipeHandle);
		/* MaximumTransferSize */
		Stream_Write_UINT32(out, MsPipe->MaximumTransferSize);
		/* PipeFlags */
		Stream_Write_UINT32(out, MsPipe->PipeFlags);
	}

	return TRUE;
}