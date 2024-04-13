static BOOL rdp_read_draw_gdiplus_cache_capability_set(wStream* s, UINT16 length,
                                                       rdpSettings* settings)
{
	UINT32 drawGDIPlusSupportLevel;
	UINT32 drawGdiplusCacheLevel;

	if (length < 40)
		return FALSE;

	Stream_Read_UINT32(s, drawGDIPlusSupportLevel); /* drawGDIPlusSupportLevel (4 bytes) */
	Stream_Seek_UINT32(s);                          /* GdipVersion (4 bytes) */
	Stream_Read_UINT32(s, drawGdiplusCacheLevel);   /* drawGdiplusCacheLevel (4 bytes) */
	Stream_Seek(s, 10);                             /* GdipCacheEntries (10 bytes) */
	Stream_Seek(s, 8);                              /* GdipCacheChunkSize (8 bytes) */
	Stream_Seek(s, 6);                              /* GdipImageCacheProperties (6 bytes) */

	if (drawGDIPlusSupportLevel & DRAW_GDIPLUS_SUPPORTED)
		settings->DrawGdiPlusEnabled = TRUE;

	if (drawGdiplusCacheLevel & DRAW_GDIPLUS_CACHE_LEVEL_ONE)
		settings->DrawGdiPlusCacheEnabled = TRUE;

	return TRUE;
}