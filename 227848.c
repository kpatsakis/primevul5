static BOOL rdp_print_frame_acknowledge_capability_set(wStream* s, UINT16 length)
{
	UINT32 frameAcknowledge;
	WLog_INFO(TAG, "FrameAcknowledgeCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT32(s, frameAcknowledge); /* frameAcknowledge (4 bytes) */
	WLog_INFO(TAG, "\tframeAcknowledge: 0x%08" PRIX32 "", frameAcknowledge);
	return TRUE;
}