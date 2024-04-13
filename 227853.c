static BOOL rdp_read_color_cache_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (length < 8)
		return FALSE;

	Stream_Seek_UINT16(s); /* colorTableCacheSize (2 bytes) */
	Stream_Seek_UINT16(s); /* pad2Octets (2 bytes) */
	return TRUE;
}