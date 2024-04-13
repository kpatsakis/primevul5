static BOOL rdp_print_offscreen_bitmap_cache_capability_set(wStream* s, UINT16 length)
{
	UINT32 offscreenSupportLevel;
	UINT16 offscreenCacheSize;
	UINT16 offscreenCacheEntries;
	WLog_INFO(TAG, "OffscreenBitmapCacheCapabilitySet (length %" PRIu16 "):", length);

	if (length < 12)
		return FALSE;

	Stream_Read_UINT32(s, offscreenSupportLevel); /* offscreenSupportLevel (4 bytes) */
	Stream_Read_UINT16(s, offscreenCacheSize);    /* offscreenCacheSize (2 bytes) */
	Stream_Read_UINT16(s, offscreenCacheEntries); /* offscreenCacheEntries (2 bytes) */
	WLog_INFO(TAG, "\toffscreenSupportLevel: 0x%08" PRIX32 "", offscreenSupportLevel);
	WLog_INFO(TAG, "\toffscreenCacheSize: 0x%04" PRIX16 "", offscreenCacheSize);
	WLog_INFO(TAG, "\toffscreenCacheEntries: 0x%04" PRIX16 "", offscreenCacheEntries);
	return TRUE;
}