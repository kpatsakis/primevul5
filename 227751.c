static BOOL rdp_print_brush_capability_set(wStream* s, UINT16 length)
{
	UINT32 brushSupportLevel;
	WLog_INFO(TAG, "BrushCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, brushSupportLevel); /* brushSupportLevel (4 bytes) */
	WLog_INFO(TAG, "\tbrushSupportLevel: 0x%08" PRIX32 "", brushSupportLevel);
	return TRUE;
}