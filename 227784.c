static BOOL rdp_read_control_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (length < 12)
		return FALSE;

	Stream_Seek_UINT16(s); /* controlFlags (2 bytes) */
	Stream_Seek_UINT16(s); /* remoteDetachFlag (2 bytes) */
	Stream_Seek_UINT16(s); /* controlInterest (2 bytes) */
	Stream_Seek_UINT16(s); /* detachInterest (2 bytes) */
	return TRUE;
}