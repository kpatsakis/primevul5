static BOOL rdp_read_desktop_composition_capability_set(wStream* s, UINT16 length,
                                                        rdpSettings* settings)
{
	if (length < 6)
		return FALSE;

	Stream_Seek_UINT16(s); /* compDeskSupportLevel (2 bytes) */
	return TRUE;
}