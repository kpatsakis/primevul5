static BOOL rdp_write_virtual_channel_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT32 flags;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	flags = VCCAPS_NO_COMPR;
	Stream_Write_UINT32(s, flags);                             /* flags (4 bytes) */
	Stream_Write_UINT32(s, settings->VirtualChannelChunkSize); /* VCChunkSize (4 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_VIRTUAL_CHANNEL);
	return TRUE;
}