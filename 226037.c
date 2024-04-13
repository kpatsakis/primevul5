UINT cliprdr_read_file_contents_response(wStream* s, CLIPRDR_FILE_CONTENTS_RESPONSE* response)
{
	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "not enough remaining data");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT32(s, response->streamId);   /* streamId (4 bytes) */
	response->requestedData = Stream_Pointer(s); /* requestedFileContentsData */
	response->cbRequested = response->dataLen - 4;
	return CHANNEL_RC_OK;
}