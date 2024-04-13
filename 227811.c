static BOOL rdp_print_desktop_composition_capability_set(wStream* s, UINT16 length)
{
	UINT16 compDeskSupportLevel;
	WLog_INFO(TAG, "DesktopCompositionCapabilitySet (length %" PRIu16 "):", length);

	if (length < 6)
		return FALSE;

	Stream_Read_UINT16(s, compDeskSupportLevel); /* compDeskSupportLevel (2 bytes) */
	WLog_INFO(TAG, "\tcompDeskSupportLevel: 0x%04" PRIX16 "", compDeskSupportLevel);
	return TRUE;
}