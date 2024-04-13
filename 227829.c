static BOOL rdp_write_remote_programs_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT32 railSupportLevel;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	header = rdp_capability_set_start(s);
	railSupportLevel = RAIL_LEVEL_SUPPORTED;

	if (settings->RemoteApplicationSupportLevel & RAIL_LEVEL_DOCKED_LANGBAR_SUPPORTED)
	{
		if (settings->RemoteAppLanguageBarSupported)
			railSupportLevel |= RAIL_LEVEL_DOCKED_LANGBAR_SUPPORTED;
	}

	railSupportLevel |= RAIL_LEVEL_SHELL_INTEGRATION_SUPPORTED;
	railSupportLevel |= RAIL_LEVEL_LANGUAGE_IME_SYNC_SUPPORTED;
	railSupportLevel |= RAIL_LEVEL_SERVER_TO_CLIENT_IME_SYNC_SUPPORTED;
	railSupportLevel |= RAIL_LEVEL_HIDE_MINIMIZED_APPS_SUPPORTED;
	railSupportLevel |= RAIL_LEVEL_WINDOW_CLOAKING_SUPPORTED;
	railSupportLevel |= RAIL_LEVEL_HANDSHAKE_EX_SUPPORTED;
	/* Mask out everything the server does not support. */
	railSupportLevel &= settings->RemoteApplicationSupportLevel;
	Stream_Write_UINT32(s, railSupportLevel); /* railSupportLevel (4 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_RAIL);
	return TRUE;
}