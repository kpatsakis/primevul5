static BOOL rdp_print_multifragment_update_capability_set(wStream* s, UINT16 length)
{
	UINT32 maxRequestSize;
	WLog_INFO(TAG, "MultifragmentUpdateCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, maxRequestSize); /* maxRequestSize (4 bytes) */
	WLog_INFO(TAG, "\tmaxRequestSize: 0x%08" PRIX32 "", maxRequestSize);
	return TRUE;
}