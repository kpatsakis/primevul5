MSUSB_CONFIG_DESCRIPTOR* msusb_msconfig_read(wStream* s, UINT32 NumInterfaces)
{
	MSUSB_CONFIG_DESCRIPTOR* MsConfig;
	BYTE lenConfiguration, typeConfiguration;

	if (Stream_GetRemainingCapacity(s) < 6ULL + NumInterfaces * 2ULL)
		return NULL;

	MsConfig = msusb_msconfig_new();

	if (!MsConfig)
		goto fail;

	MsConfig->MsInterfaces = msusb_msinterface_read_list(s, NumInterfaces);

	if (!MsConfig->MsInterfaces)
		goto fail;

	Stream_Read_UINT8(s, lenConfiguration);
	Stream_Read_UINT8(s, typeConfiguration);

	if (lenConfiguration != 0x9 || typeConfiguration != 0x2)
	{
		WLog_ERR(TAG, "len and type must be 0x9 and 0x2 , but it is 0x%" PRIx8 " and 0x%" PRIx8 "",
		         lenConfiguration, typeConfiguration);
		goto fail;
	}

	Stream_Read_UINT16(s, MsConfig->wTotalLength);
	Stream_Seek(s, 1);
	Stream_Read_UINT8(s, MsConfig->bConfigurationValue);
	MsConfig->NumInterfaces = NumInterfaces;
	return MsConfig;
fail:
	msusb_msconfig_free(MsConfig);
	return NULL;
}