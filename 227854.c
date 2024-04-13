static BOOL rdp_read_window_activation_capability_set(wStream* s, UINT16 length,
                                                      rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (length < 12)
		return FALSE;

	Stream_Seek_UINT16(s); /* helpKeyFlag (2 bytes) */
	Stream_Seek_UINT16(s); /* helpKeyIndexFlag (2 bytes) */
	Stream_Seek_UINT16(s); /* helpExtendedKeyFlag (2 bytes) */
	Stream_Seek_UINT16(s); /* windowManagerKeyFlag (2 bytes) */
	return TRUE;
}