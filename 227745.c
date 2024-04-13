static BOOL rdp_print_bitmap_cache_host_support_capability_set(wStream* s, UINT16 length)
{
	BYTE cacheVersion;
	BYTE pad1;
	UINT16 pad2;
	WLog_INFO(TAG, "BitmapCacheHostSupportCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT8(s, cacheVersion); /* cacheVersion (1 byte) */
	Stream_Read_UINT8(s, pad1);         /* pad1 (1 byte) */
	Stream_Read_UINT16(s, pad2);        /* pad2 (2 bytes) */
	WLog_INFO(TAG, "\tcacheVersion: 0x%02" PRIX8 "", cacheVersion);
	WLog_INFO(TAG, "\tpad1: 0x%02" PRIX8 "", pad1);
	WLog_INFO(TAG, "\tpad2: 0x%04" PRIX16 "", pad2);
	return TRUE;
}