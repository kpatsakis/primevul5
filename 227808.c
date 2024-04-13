static BOOL rdp_print_window_list_capability_set(wStream* s, UINT16 length)
{
	UINT32 wndSupportLevel;
	BYTE numIconCaches;
	UINT16 numIconCacheEntries;
	WLog_INFO(TAG, "WindowListCapabilitySet (length %" PRIu16 "):", length);

	if (length < 11)
		return FALSE;

	Stream_Read_UINT32(s, wndSupportLevel);     /* wndSupportLevel (4 bytes) */
	Stream_Read_UINT8(s, numIconCaches);        /* numIconCaches (1 byte) */
	Stream_Read_UINT16(s, numIconCacheEntries); /* numIconCacheEntries (2 bytes) */
	WLog_INFO(TAG, "\twndSupportLevel: 0x%08" PRIX32 "", wndSupportLevel);
	WLog_INFO(TAG, "\tnumIconCaches: 0x%02" PRIX8 "", numIconCaches);
	WLog_INFO(TAG, "\tnumIconCacheEntries: 0x%04" PRIX16 "", numIconCacheEntries);
	return TRUE;
}