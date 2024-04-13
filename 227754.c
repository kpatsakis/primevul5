static BOOL rdp_read_multifragment_update_capability_set(wStream* s, UINT16 length,
                                                         rdpSettings* settings)
{
	UINT32 multifragMaxRequestSize;

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, multifragMaxRequestSize); /* MaxRequestSize (4 bytes) */

	if (settings->ServerMode)
	{
		/*
		 * Special case: The client announces multifragment update support but sets the maximum
		 * request size to something smaller than maximum size for *one* fast-path PDU. In this case
		 * behave like no multifragment updates were supported and make sure no fragmentation
		 * happens by setting FASTPATH_FRAGMENT_SAFE_SIZE.
		 *
		 * This behaviour was observed with some windows ce rdp clients.
		 */
		if (multifragMaxRequestSize < FASTPATH_MAX_PACKET_SIZE)
			multifragMaxRequestSize = FASTPATH_FRAGMENT_SAFE_SIZE;

		if (settings->RemoteFxCodec)
		{
			/**
			 * If we are using RemoteFX the client MUST use a value greater
			 * than or equal to the value we've previously sent in the server to
			 * client multi-fragment update capability set (MS-RDPRFX 1.5)
			 */
			if (multifragMaxRequestSize < settings->MultifragMaxRequestSize)
			{
				/**
				 * If it happens to be smaller we honor the client's value but
				 * have to disable RemoteFX
				 */
				settings->RemoteFxCodec = FALSE;
				settings->MultifragMaxRequestSize = multifragMaxRequestSize;
			}
			else
			{
				/* no need to increase server's max request size setting here */
			}
		}
		else
		{
			settings->MultifragMaxRequestSize = multifragMaxRequestSize;
		}
	}
	else
	{
		/**
		 * In client mode we keep up with the server's capabilites.
		 * In RemoteFX mode we MUST do this but it might also be useful to
		 * receive larger related bitmap updates.
		 */
		if (multifragMaxRequestSize > settings->MultifragMaxRequestSize)
			settings->MultifragMaxRequestSize = multifragMaxRequestSize;
	}

	return TRUE;
}