static BOOL rdp_read_surface_commands_capability_set(wStream* s, UINT16 length,
                                                     rdpSettings* settings)
{
	UINT32 cmdFlags;

	if (length < 12)
		return FALSE;

	Stream_Read_UINT32(s, cmdFlags); /* cmdFlags (4 bytes) */
	Stream_Seek_UINT32(s);           /* reserved (4 bytes) */
	settings->SurfaceCommandsEnabled = TRUE;
	settings->SurfaceFrameMarkerEnabled = (cmdFlags & SURFCMDS_FRAME_MARKER) ? TRUE : FALSE;
	return TRUE;
}