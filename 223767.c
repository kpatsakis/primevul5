void msusb_msconfig_free(MSUSB_CONFIG_DESCRIPTOR* MsConfig)
{
	if (MsConfig)
	{
		msusb_msinterface_free_list(MsConfig->MsInterfaces, MsConfig->NumInterfaces);
		MsConfig->MsInterfaces = NULL;
		free(MsConfig);
	}
}