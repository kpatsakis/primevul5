static BOOL cliprdr_validate_file_contents_request(const CLIPRDR_FILE_CONTENTS_REQUEST* request)
{
	/*
	 * [MS-RDPECLIP] 2.2.5.3 File Contents Request PDU (CLIPRDR_FILECONTENTS_REQUEST).
	 *
	 * A request for the size of the file identified by the lindex field. The size MUST be
	 * returned as a 64-bit, unsigned integer. The cbRequested field MUST be set to
	 * 0x00000008 and both the nPositionLow and nPositionHigh fields MUST be
	 * set to 0x00000000.
	 */

	if (request->dwFlags & FILECONTENTS_SIZE)
	{
		if (request->cbRequested != sizeof(UINT64))
		{
			WLog_ERR(TAG, "[%s]: cbRequested must be %" PRIu32 ", got %" PRIu32 "", __FUNCTION__,
			         sizeof(UINT64), request->cbRequested);
			return FALSE;
		}

		if (request->nPositionHigh != 0 || request->nPositionLow != 0)
		{
			WLog_ERR(TAG, "[%s]: nPositionHigh and nPositionLow must be set to 0", __FUNCTION__);
			return FALSE;
		}
	}

	return TRUE;
}