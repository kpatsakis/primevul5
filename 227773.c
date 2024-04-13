static BOOL rdp_read_brush_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (length < 8)
		return FALSE;

	Stream_Seek_UINT32(s); /* brushSupportLevel (4 bytes) */
	return TRUE;
}