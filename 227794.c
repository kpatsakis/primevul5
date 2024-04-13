static BOOL rdp_write_bitmap_cache_v3_codec_id_capability_set(wStream* s,
                                                              const rdpSettings* settings)
{
	size_t header;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	if (settings->BitmapCacheV3CodecId > UINT8_MAX)
		return FALSE;
	Stream_Write_UINT8(s, (UINT8)settings->BitmapCacheV3CodecId);
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_BITMAP_CACHE_V3_CODEC_ID);
	return TRUE;
}