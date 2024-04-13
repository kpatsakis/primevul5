static BOOL rdp_read_remote_programs_capability_set(wStream* s, UINT16 length,
                                                    rdpSettings* settings)
{
	UINT32 railSupportLevel;

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, railSupportLevel); /* railSupportLevel (4 bytes) */

	if ((railSupportLevel & RAIL_LEVEL_SUPPORTED) == 0)
	{
		if (settings->RemoteApplicationMode == TRUE)
		{
			/* RemoteApp Failure! */
			settings->RemoteApplicationMode = FALSE;
		}
	}

	/* 2.2.2.2.3 HandshakeEx PDU (TS_RAIL_ORDER_HANDSHAKE_EX)
	 * the handshake ex pdu is supported when both, client and server announce
	 * it OR if we are ready to begin enhanced remoteAPP mode. */
	if (settings->RemoteApplicationMode)
		railSupportLevel |= RAIL_LEVEL_HANDSHAKE_EX_SUPPORTED;

	settings->RemoteApplicationSupportLevel =
	    railSupportLevel & settings->RemoteApplicationSupportMask;
	return TRUE;
}