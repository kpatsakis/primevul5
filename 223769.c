BOOL msusb_mspipes_replace(MSUSB_INTERFACE_DESCRIPTOR* MsInterface,
                           MSUSB_PIPE_DESCRIPTOR** NewMsPipes, UINT32 NewNumberOfPipes)
{
	if (!MsInterface || !NewMsPipes)
		return FALSE;

	/* free orignal MsPipes */
	msusb_mspipes_free(MsInterface->MsPipes, MsInterface->NumberOfPipes);
	/* And replace it */
	MsInterface->MsPipes = NewMsPipes;
	MsInterface->NumberOfPipes = NewNumberOfPipes;
	return TRUE;
}