UINT cliprdr_read_format_data_request(wStream* s, CLIPRDR_FORMAT_DATA_REQUEST* request)
{
	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "not enough data in stream!");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT32(s, request->requestedFormatId); /* requestedFormatId (4 bytes) */
	return CHANNEL_RC_OK;
}