static BOOL rdp_print_bitmap_codecs_capability_set(wStream* s, UINT16 length)
{
	GUID codecGuid;
	BYTE bitmapCodecCount;
	BYTE codecId;
	UINT16 codecPropertiesLength;
	UINT16 remainingLength;
	WLog_INFO(TAG, "BitmapCodecsCapabilitySet (length %" PRIu16 "):", length);

	if (length < 5)
		return FALSE;

	Stream_Read_UINT8(s, bitmapCodecCount); /* bitmapCodecCount (1 byte) */
	remainingLength = length - 5;
	WLog_INFO(TAG, "\tbitmapCodecCount: %" PRIu8 "", bitmapCodecCount);

	while (bitmapCodecCount > 0)
	{
		if (remainingLength < 19)
			return FALSE;

		rdp_read_bitmap_codec_guid(s, &codecGuid); /* codecGuid (16 bytes) */
		Stream_Read_UINT8(s, codecId);             /* codecId (1 byte) */
		WLog_INFO(TAG, "\tcodecGuid: 0x");
		rdp_print_bitmap_codec_guid(&codecGuid);
		WLog_INFO(TAG, " (%s)", rdp_get_bitmap_codec_guid_name(&codecGuid));
		WLog_INFO(TAG, "\tcodecId: %" PRIu8 "", codecId);
		Stream_Read_UINT16(s, codecPropertiesLength); /* codecPropertiesLength (2 bytes) */
		WLog_INFO(TAG, "\tcodecPropertiesLength: %" PRIu16 "", codecPropertiesLength);
		remainingLength -= 19;

		if (remainingLength < codecPropertiesLength)
			return FALSE;

		Stream_Seek(s, codecPropertiesLength); /* codecProperties */
		remainingLength -= codecPropertiesLength;
		bitmapCodecCount--;
	}

	return TRUE;
}