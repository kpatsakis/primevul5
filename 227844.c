static BOOL rdp_write_draw_nine_grid_cache_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT32 drawNineGridSupportLevel;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	drawNineGridSupportLevel =
	    (settings->DrawNineGridEnabled) ? DRAW_NINEGRID_SUPPORTED_V2 : DRAW_NINEGRID_NO_SUPPORT;
	Stream_Write_UINT32(s, drawNineGridSupportLevel); /* drawNineGridSupportLevel (4 bytes) */
	Stream_Write_UINT16(s, settings->DrawNineGridCacheSize); /* drawNineGridCacheSize (2 bytes) */
	Stream_Write_UINT16(
	    s, settings->DrawNineGridCacheEntries); /* drawNineGridCacheEntries (2 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_DRAW_NINE_GRID_CACHE);
	return TRUE;
}