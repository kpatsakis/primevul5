UINT cliprdr_read_unlock_clipdata(wStream* s, CLIPRDR_UNLOCK_CLIPBOARD_DATA* unlockClipboardData)
{
	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "not enough remaining data");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT32(s, unlockClipboardData->clipDataId); /* clipDataId (4 bytes) */
	return CHANNEL_RC_OK;
}