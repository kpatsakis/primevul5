static BOOL rdp_read_large_pointer_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	UINT16 largePointerSupportFlags;

	if (length < 6)
		return FALSE;

	Stream_Read_UINT16(s, largePointerSupportFlags); /* largePointerSupportFlags (2 bytes) */
	settings->LargePointerFlag =
	    largePointerSupportFlags & (LARGE_POINTER_FLAG_96x96 | LARGE_POINTER_FLAG_384x384);
	if ((largePointerSupportFlags & ~(LARGE_POINTER_FLAG_96x96 | LARGE_POINTER_FLAG_384x384)) != 0)
	{
		WLog_WARN(
		    TAG,
		    "TS_LARGE_POINTER_CAPABILITYSET with unsupported flags %04X (all flags %04X) received",
		    largePointerSupportFlags & ~(LARGE_POINTER_FLAG_96x96 | LARGE_POINTER_FLAG_384x384),
		    largePointerSupportFlags);
	}
	return TRUE;
}