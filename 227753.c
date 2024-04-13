static BOOL rdp_print_virtual_channel_capability_set(wStream* s, UINT16 length)
{
	UINT32 flags;
	UINT32 VCChunkSize;
	WLog_INFO(TAG, "VirtualChannelCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, flags); /* flags (4 bytes) */

	if (length > 8)
		Stream_Read_UINT32(s, VCChunkSize); /* VCChunkSize (4 bytes) */
	else
		VCChunkSize = 1600;

	WLog_INFO(TAG, "\tflags: 0x%08" PRIX32 "", flags);
	WLog_INFO(TAG, "\tVCChunkSize: 0x%08" PRIX32 "", VCChunkSize);
	return TRUE;
}