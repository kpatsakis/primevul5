static BOOL rdp_write_share_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT16 nodeId;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	nodeId = (settings->ServerMode) ? 0x03EA : 0;
	Stream_Write_UINT16(s, nodeId); /* nodeId (2 bytes) */
	Stream_Write_UINT16(s, 0);      /* pad2Octets (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_SHARE);
	return TRUE;
}