static BOOL rdp_write_jpeg_client_capability_container(wStream* s, const rdpSettings* settings)
{
	if (!Stream_EnsureRemainingCapacity(s, 8))
		return FALSE;

	Stream_Write_UINT16(s, 1); /* codecPropertiesLength */
	Stream_Write_UINT8(s, settings->JpegQuality);
	return TRUE;
}