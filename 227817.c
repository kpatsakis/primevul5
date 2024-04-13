static BOOL rdp_write_rfx_server_capability_container(wStream* s, const rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (!Stream_EnsureRemainingCapacity(s, 8))
		return FALSE;

	Stream_Write_UINT16(s, 4); /* codecPropertiesLength */
	Stream_Write_UINT32(s, 0); /* reserved */
	return TRUE;
}