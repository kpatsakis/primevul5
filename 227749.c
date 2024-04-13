static BOOL rdp_print_draw_nine_grid_cache_capability_set(wStream* s, UINT16 length)
{
	UINT32 drawNineGridSupportLevel;
	UINT16 DrawNineGridCacheSize;
	UINT16 DrawNineGridCacheEntries;
	WLog_INFO(TAG, "DrawNineGridCacheCapabilitySet (length %" PRIu16 "):", length);

	if (length < 12)
		return FALSE;

	Stream_Read_UINT32(s, drawNineGridSupportLevel); /* drawNineGridSupportLevel (4 bytes) */
	Stream_Read_UINT16(s, DrawNineGridCacheSize);    /* drawNineGridCacheSize (2 bytes) */
	Stream_Read_UINT16(s, DrawNineGridCacheEntries); /* drawNineGridCacheEntries (2 bytes) */
	return TRUE;
}