static BOOL rdp_print_sound_capability_set(wStream* s, UINT16 length)
{
	UINT16 soundFlags;
	UINT16 pad2OctetsA;
	WLog_INFO(TAG, "SoundCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT16(s, soundFlags);  /* soundFlags (2 bytes) */
	Stream_Read_UINT16(s, pad2OctetsA); /* pad2OctetsA (2 bytes) */
	WLog_INFO(TAG, "\tsoundFlags: 0x%04" PRIX16 "", soundFlags);
	WLog_INFO(TAG, "\tpad2OctetsA: 0x%04" PRIX16 "", pad2OctetsA);
	return TRUE;
}