static BOOL rdp_print_bitmap_cache_v3_codec_id_capability_set(wStream* s, UINT16 length)
{
	BYTE bitmapCacheV3CodecId;
	WLog_INFO(TAG, "BitmapCacheV3CodecIdCapabilitySet (length %" PRIu16 "):", length);

	if (length < 5)
		return FALSE;

	Stream_Read_UINT8(s, bitmapCacheV3CodecId); /* bitmapCacheV3CodecId (1 byte) */
	WLog_INFO(TAG, "\tbitmapCacheV3CodecId: 0x%02" PRIX8 "", bitmapCacheV3CodecId);
	return TRUE;
}