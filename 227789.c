static BOOL rdp_print_font_capability_set(wStream* s, UINT16 length)
{
	UINT16 fontSupportFlags = 0;
	UINT16 pad2Octets = 0;
	WLog_INFO(TAG, "FontCapabilitySet (length %" PRIu16 "):", length);

	if (length > 4)
		Stream_Read_UINT16(s, fontSupportFlags); /* fontSupportFlags (2 bytes) */

	if (length > 6)
		Stream_Read_UINT16(s, pad2Octets); /* pad2Octets (2 bytes) */

	WLog_INFO(TAG, "\tfontSupportFlags: 0x%04" PRIX16 "", fontSupportFlags);
	WLog_INFO(TAG, "\tpad2Octets: 0x%04" PRIX16 "", pad2Octets);
	return TRUE;
}