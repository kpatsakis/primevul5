static BOOL rdp_write_multifragment_update_capability_set(wStream* s, rdpSettings* settings)
{
	size_t header;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	if (settings->ServerMode && settings->MultifragMaxRequestSize == 0)
	{
		/**
		 * In server mode we prefer to use the highest useful request size that
		 * will allow us to pack a complete screen update into a single fast
		 * path PDU using any of the supported codecs.
		 * However, the client is completely free to accept our proposed
		 * max request size or send a different value in the client-to-server
		 * multi-fragment update capability set and we have to accept that,
		 * unless we are using RemoteFX where the client MUST announce a value
		 * greater than or equal to the value we're sending here.
		 * See [MS-RDPRFX 1.5 capability #2]
		 */
		UINT32 tileNumX = (settings->DesktopWidth + 63) / 64;
		UINT32 tileNumY = (settings->DesktopHeight + 63) / 64;
		settings->MultifragMaxRequestSize = tileNumX * tileNumY * 16384;
		/* and add room for headers, regions, frame markers, etc. */
		settings->MultifragMaxRequestSize += 16384;
	}

	header = rdp_capability_set_start(s);
	Stream_Write_UINT32(s, settings->MultifragMaxRequestSize); /* MaxRequestSize (4 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_MULTI_FRAGMENT_UPDATE);
	return TRUE;
}