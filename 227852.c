static BOOL rdp_print_color_cache_capability_set(wStream* s, UINT16 length)
{
	UINT16 colorTableCacheSize;
	UINT16 pad2Octets;
	WLog_INFO(TAG, "ColorCacheCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT16(s, colorTableCacheSize); /* colorTableCacheSize (2 bytes) */
	Stream_Read_UINT16(s, pad2Octets);          /* pad2Octets (2 bytes) */
	WLog_INFO(TAG, "\tcolorTableCacheSize: 0x%04" PRIX16 "", colorTableCacheSize);
	WLog_INFO(TAG, "\tpad2Octets: 0x%04" PRIX16 "", pad2Octets);
	return TRUE;
}