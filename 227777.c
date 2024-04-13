static BOOL rdp_print_surface_commands_capability_set(wStream* s, UINT16 length)
{
	UINT32 cmdFlags;
	UINT32 reserved;
	WLog_INFO(TAG, "SurfaceCommandsCapabilitySet (length %" PRIu16 "):", length);

	if (length < 12)
		return FALSE;

	Stream_Read_UINT32(s, cmdFlags); /* cmdFlags (4 bytes) */
	Stream_Read_UINT32(s, reserved); /* reserved (4 bytes) */
	WLog_INFO(TAG, "\tcmdFlags: 0x%08" PRIX32 "", cmdFlags);
	WLog_INFO(TAG, "\treserved: 0x%08" PRIX32 "", reserved);
	return TRUE;
}