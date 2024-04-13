static BOOL rdp_read_frame_acknowledge_capability_set(wStream* s, UINT16 length,
                                                      rdpSettings* settings)
{
	if (length < 8)
		return FALSE;

	if (settings->ServerMode)
	{
		Stream_Read_UINT32(s, settings->FrameAcknowledge); /* (4 bytes) */
	}
	else
	{
		Stream_Seek_UINT32(s); /* (4 bytes) */
	}

	return TRUE;
}