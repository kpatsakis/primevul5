static BOOL rdp_write_window_activation_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;

	WINPR_UNUSED(settings);
	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	Stream_Write_UINT16(s, 0); /* helpKeyFlag (2 bytes) */
	Stream_Write_UINT16(s, 0); /* helpKeyIndexFlag (2 bytes) */
	Stream_Write_UINT16(s, 0); /* helpExtendedKeyFlag (2 bytes) */
	Stream_Write_UINT16(s, 0); /* windowManagerKeyFlag (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_ACTIVATION);
	return TRUE;
}