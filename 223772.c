void msusb_msconfig_dump(MSUSB_CONFIG_DESCRIPTOR* MsConfig)
{
	MSUSB_INTERFACE_DESCRIPTOR** MsInterfaces;
	MSUSB_INTERFACE_DESCRIPTOR* MsInterface;
	MSUSB_PIPE_DESCRIPTOR** MsPipes;
	MSUSB_PIPE_DESCRIPTOR* MsPipe;
	UINT32 inum = 0, pnum = 0;
	WLog_INFO(TAG, "=================MsConfig:========================");
	WLog_INFO(TAG, "wTotalLength:%" PRIu16 "", MsConfig->wTotalLength);
	WLog_INFO(TAG, "bConfigurationValue:%" PRIu8 "", MsConfig->bConfigurationValue);
	WLog_INFO(TAG, "ConfigurationHandle:0x%08" PRIx32 "", MsConfig->ConfigurationHandle);
	WLog_INFO(TAG, "InitCompleted:%d", MsConfig->InitCompleted);
	WLog_INFO(TAG, "MsOutSize:%d", MsConfig->MsOutSize);
	WLog_INFO(TAG, "NumInterfaces:%" PRIu32 "", MsConfig->NumInterfaces);
	MsInterfaces = MsConfig->MsInterfaces;

	for (inum = 0; inum < MsConfig->NumInterfaces; inum++)
	{
		MsInterface = MsInterfaces[inum];
		WLog_INFO(TAG, "	Interface: %" PRIu8 "", MsInterface->InterfaceNumber);
		WLog_INFO(TAG, "	Length: %" PRIu16 "", MsInterface->Length);
		WLog_INFO(TAG, "	NumberOfPipesExpected: %" PRIu16 "",
		          MsInterface->NumberOfPipesExpected);
		WLog_INFO(TAG, "	AlternateSetting: %" PRIu8 "", MsInterface->AlternateSetting);
		WLog_INFO(TAG, "	NumberOfPipes: %" PRIu32 "", MsInterface->NumberOfPipes);
		WLog_INFO(TAG, "	InterfaceHandle: 0x%08" PRIx32 "", MsInterface->InterfaceHandle);
		WLog_INFO(TAG, "	bInterfaceClass: 0x%02" PRIx8 "", MsInterface->bInterfaceClass);
		WLog_INFO(TAG, "	bInterfaceSubClass: 0x%02" PRIx8 "", MsInterface->bInterfaceSubClass);
		WLog_INFO(TAG, "	bInterfaceProtocol: 0x%02" PRIx8 "", MsInterface->bInterfaceProtocol);
		WLog_INFO(TAG, "	InitCompleted: %d", MsInterface->InitCompleted);
		MsPipes = MsInterface->MsPipes;

		for (pnum = 0; pnum < MsInterface->NumberOfPipes; pnum++)
		{
			MsPipe = MsPipes[pnum];
			WLog_INFO(TAG, "		Pipe: %d", pnum);
			WLog_INFO(TAG, "		MaximumPacketSize: 0x%04" PRIx16 "", MsPipe->MaximumPacketSize);
			WLog_INFO(TAG, "		MaximumTransferSize: 0x%08" PRIx32 "",
			          MsPipe->MaximumTransferSize);
			WLog_INFO(TAG, "		PipeFlags: 0x%08" PRIx32 "", MsPipe->PipeFlags);
			WLog_INFO(TAG, "		PipeHandle: 0x%08" PRIx32 "", MsPipe->PipeHandle);
			WLog_INFO(TAG, "		bEndpointAddress: 0x%02" PRIx8 "", MsPipe->bEndpointAddress);
			WLog_INFO(TAG, "		bInterval: %" PRIu8 "", MsPipe->bInterval);
			WLog_INFO(TAG, "		PipeType: 0x%02" PRIx8 "", MsPipe->PipeType);
			WLog_INFO(TAG, "		InitCompleted: %d", MsPipe->InitCompleted);
		}
	}

	WLog_INFO(TAG, "==================================================");
}