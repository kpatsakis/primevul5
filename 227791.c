static BOOL rdp_write_surface_commands_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT32 cmdFlags;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	cmdFlags = SURFCMDS_SET_SURFACE_BITS | SURFCMDS_STREAM_SURFACE_BITS;

	if (settings->SurfaceFrameMarkerEnabled)
		cmdFlags |= SURFCMDS_FRAME_MARKER;

	Stream_Write_UINT32(s, cmdFlags); /* cmdFlags (4 bytes) */
	Stream_Write_UINT32(s, 0);        /* reserved (4 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_SURFACE_COMMANDS);
	return TRUE;
}