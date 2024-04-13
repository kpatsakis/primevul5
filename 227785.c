static BOOL rdp_read_offscreen_bitmap_cache_capability_set(wStream* s, UINT16 length,
                                                           rdpSettings* settings)
{
	UINT32 offscreenSupportLevel;

	if (length < 12)
		return FALSE;

	Stream_Read_UINT32(s, offscreenSupportLevel);           /* offscreenSupportLevel (4 bytes) */
	Stream_Read_UINT16(s, settings->OffscreenCacheSize);    /* offscreenCacheSize (2 bytes) */
	Stream_Read_UINT16(s, settings->OffscreenCacheEntries); /* offscreenCacheEntries (2 bytes) */

	if (offscreenSupportLevel & TRUE)
		settings->OffscreenSupportLevel = TRUE;

	return TRUE;
}