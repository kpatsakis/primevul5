static BOOL rdp_print_draw_gdiplus_cache_capability_set(wStream* s, UINT16 length)
{
	UINT32 drawGdiPlusSupportLevel;
	UINT32 GdipVersion;
	UINT32 drawGdiplusCacheLevel;
	WLog_INFO(TAG, "DrawGdiPlusCacheCapabilitySet (length %" PRIu16 "):", length);

	if (length < 40)
		return FALSE;

	Stream_Read_UINT32(s, drawGdiPlusSupportLevel); /* drawGdiPlusSupportLevel (4 bytes) */
	Stream_Read_UINT32(s, GdipVersion);             /* GdipVersion (4 bytes) */
	Stream_Read_UINT32(s, drawGdiplusCacheLevel);   /* drawGdiPlusCacheLevel (4 bytes) */
	Stream_Seek(s, 10);                             /* GdipCacheEntries (10 bytes) */
	Stream_Seek(s, 8);                              /* GdipCacheChunkSize (8 bytes) */
	Stream_Seek(s, 6);                              /* GdipImageCacheProperties (6 bytes) */
	return TRUE;
}