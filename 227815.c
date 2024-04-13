static BOOL rdp_write_control_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;

	WINPR_UNUSED(settings);
	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	Stream_Write_UINT16(s, 0); /* controlFlags (2 bytes) */
	Stream_Write_UINT16(s, 0); /* remoteDetachFlag (2 bytes) */
	Stream_Write_UINT16(s, 2); /* controlInterest (2 bytes) */
	Stream_Write_UINT16(s, 2); /* detachInterest (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_CONTROL);
	return TRUE;
}