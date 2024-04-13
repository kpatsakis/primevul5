static BOOL rdp_read_window_list_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	if (length < 11)
		return FALSE;

	Stream_Read_UINT32(s, settings->RemoteWndSupportLevel); /* wndSupportLevel (4 bytes) */
	Stream_Read_UINT8(s, settings->RemoteAppNumIconCaches); /* numIconCaches (1 byte) */
	Stream_Read_UINT16(s,
	                   settings->RemoteAppNumIconCacheEntries); /* numIconCacheEntries (2 bytes) */
	return TRUE;
}