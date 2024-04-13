static BOOL rdp_write_jpeg_server_capability_container(wStream* s, const rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (!Stream_EnsureRemainingCapacity(s, 8))
		return FALSE;

	Stream_Write_UINT16(s, 1); /* codecPropertiesLength */
	Stream_Write_UINT8(s, 75);
	return TRUE;
}