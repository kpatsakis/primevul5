UINT cliprdr_read_format_data_response(wStream* s, CLIPRDR_FORMAT_DATA_RESPONSE* response)
{
	response->requestedFormatData = NULL;

	if (Stream_GetRemainingLength(s) < response->dataLen)
	{
		WLog_ERR(TAG, "not enough data in stream!");
		return ERROR_INVALID_DATA;
	}

	if (response->dataLen)
		response->requestedFormatData = Stream_Pointer(s);

	return CHANNEL_RC_OK;
}