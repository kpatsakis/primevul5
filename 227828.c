static BOOL rdp_read_bitmap_cache_v3_codec_id_capability_set(wStream* s, UINT16 length,
                                                             rdpSettings* settings)
{
	BYTE bitmapCacheV3CodecId;

	WINPR_UNUSED(settings);
	if (length < 5)
		return FALSE;

	Stream_Read_UINT8(s, bitmapCacheV3CodecId); /* bitmapCacheV3CodecId (1 byte) */
	return TRUE;
}