static BOOL rdp_read_virtual_channel_capability_set(wStream* s, UINT16 length,
                                                    rdpSettings* settings)
{
	UINT32 flags;
	UINT32 VCChunkSize;

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, flags); /* flags (4 bytes) */

	if (length > 8)
		Stream_Read_UINT32(s, VCChunkSize); /* VCChunkSize (4 bytes) */
	else
		VCChunkSize = 1600;

	if (settings->ServerMode != TRUE)
		settings->VirtualChannelChunkSize = VCChunkSize;

	return TRUE;
}