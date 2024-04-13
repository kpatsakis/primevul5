static BOOL rdp_print_large_pointer_capability_set(wStream* s, UINT16 length)
{
	UINT16 largePointerSupportFlags;
	WLog_INFO(TAG, "LargePointerCapabilitySet (length %" PRIu16 "):", length);

	if (length < 6)
		return FALSE;

	Stream_Read_UINT16(s, largePointerSupportFlags); /* largePointerSupportFlags (2 bytes) */
	WLog_INFO(TAG, "\tlargePointerSupportFlags: 0x%04" PRIX16 "", largePointerSupportFlags);
	return TRUE;
}