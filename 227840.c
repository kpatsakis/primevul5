static BOOL rdp_print_control_capability_set(wStream* s, UINT16 length)
{
	UINT16 controlFlags;
	UINT16 remoteDetachFlag;
	UINT16 controlInterest;
	UINT16 detachInterest;
	WLog_INFO(TAG, "ControlCapabilitySet (length %" PRIu16 "):", length);

	if (length < 12)
		return FALSE;

	Stream_Read_UINT16(s, controlFlags);     /* controlFlags (2 bytes) */
	Stream_Read_UINT16(s, remoteDetachFlag); /* remoteDetachFlag (2 bytes) */
	Stream_Read_UINT16(s, controlInterest);  /* controlInterest (2 bytes) */
	Stream_Read_UINT16(s, detachInterest);   /* detachInterest (2 bytes) */
	WLog_INFO(TAG, "\tcontrolFlags: 0x%04" PRIX16 "", controlFlags);
	WLog_INFO(TAG, "\tremoteDetachFlag: 0x%04" PRIX16 "", remoteDetachFlag);
	WLog_INFO(TAG, "\tcontrolInterest: 0x%04" PRIX16 "", controlInterest);
	WLog_INFO(TAG, "\tdetachInterest: 0x%04" PRIX16 "", detachInterest);
	return TRUE;
}