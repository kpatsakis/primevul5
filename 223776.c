static void msusb_msinterface_free(MSUSB_INTERFACE_DESCRIPTOR* MsInterface)
{
	if (MsInterface)
	{
		msusb_mspipes_free(MsInterface->MsPipes, MsInterface->NumberOfPipes);
		MsInterface->MsPipes = NULL;
		free(MsInterface);
	}
}