static BOOL rdp_write_offscreen_bitmap_cache_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT32 offscreenSupportLevel = 0x00;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	if (settings->OffscreenSupportLevel)
	{
		offscreenSupportLevel = 0x01;
		Stream_Write_UINT32(s, offscreenSupportLevel);        /* offscreenSupportLevel (4 bytes) */
		Stream_Write_UINT16(s, settings->OffscreenCacheSize); /* offscreenCacheSize (2 bytes) */
		Stream_Write_UINT16(s,
		                    settings->OffscreenCacheEntries); /* offscreenCacheEntries (2 bytes) */
	}
	else
		Stream_Zero(s, 8);

	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_OFFSCREEN_CACHE);
	return TRUE;
}