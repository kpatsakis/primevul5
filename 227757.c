static BOOL rdp_print_remote_programs_capability_set(wStream* s, UINT16 length)
{
	UINT32 railSupportLevel;
	WLog_INFO(TAG, "RemoteProgramsCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, railSupportLevel); /* railSupportLevel (4 bytes) */
	WLog_INFO(TAG, "\trailSupportLevel: 0x%08" PRIX32 "", railSupportLevel);
	return TRUE;
}