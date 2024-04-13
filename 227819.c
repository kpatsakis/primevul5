static BOOL rdp_read_general_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	UINT16 extraFlags;
	BYTE refreshRectSupport;
	BYTE suppressOutputSupport;

	if (length < 24)
		return FALSE;

	if (settings->ServerMode)
	{
		Stream_Read_UINT16(s, settings->OsMajorType); /* osMajorType (2 bytes) */
		Stream_Read_UINT16(s, settings->OsMinorType); /* osMinorType (2 bytes) */
	}
	else
	{
		Stream_Seek_UINT16(s); /* osMajorType (2 bytes) */
		Stream_Seek_UINT16(s); /* osMinorType (2 bytes) */
	}

	Stream_Seek_UINT16(s);                       /* protocolVersion (2 bytes) */
	Stream_Seek_UINT16(s);                       /* pad2OctetsA (2 bytes) */
	Stream_Seek_UINT16(s);                       /* generalCompressionTypes (2 bytes) */
	Stream_Read_UINT16(s, extraFlags);           /* extraFlags (2 bytes) */
	Stream_Seek_UINT16(s);                       /* updateCapabilityFlag (2 bytes) */
	Stream_Seek_UINT16(s);                       /* remoteUnshareFlag (2 bytes) */
	Stream_Seek_UINT16(s);                       /* generalCompressionLevel (2 bytes) */
	Stream_Read_UINT8(s, refreshRectSupport);    /* refreshRectSupport (1 byte) */
	Stream_Read_UINT8(s, suppressOutputSupport); /* suppressOutputSupport (1 byte) */
	settings->NoBitmapCompressionHeader = (extraFlags & NO_BITMAP_COMPRESSION_HDR) ? TRUE : FALSE;
	settings->LongCredentialsSupported = (extraFlags & LONG_CREDENTIALS_SUPPORTED) ? TRUE : FALSE;

	if (!(extraFlags & FASTPATH_OUTPUT_SUPPORTED))
		settings->FastPathOutput = FALSE;

	if (!(extraFlags & ENC_SALTED_CHECKSUM))
		settings->SaltedChecksum = FALSE;

	if (!settings->ServerMode)
	{
		/**
		 * Note: refreshRectSupport and suppressOutputSupport are
		 * server-only flags indicating to the client weather the
		 * respective PDUs are supported. See MS-RDPBCGR 2.2.7.1.1
		 */
		if (!refreshRectSupport)
			settings->RefreshRect = FALSE;

		if (!suppressOutputSupport)
			settings->SuppressOutput = FALSE;
	}

	return TRUE;
}