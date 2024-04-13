static BOOL rdp_print_general_capability_set(wStream* s, UINT16 length)
{
	UINT16 osMajorType;
	UINT16 osMinorType;
	UINT16 protocolVersion;
	UINT16 pad2OctetsA;
	UINT16 generalCompressionTypes;
	UINT16 extraFlags;
	UINT16 updateCapabilityFlag;
	UINT16 remoteUnshareFlag;
	UINT16 generalCompressionLevel;
	BYTE refreshRectSupport;
	BYTE suppressOutputSupport;

	if (length < 24)
		return FALSE;

	WLog_INFO(TAG, "GeneralCapabilitySet (length %" PRIu16 "):", length);
	Stream_Read_UINT16(s, osMajorType);             /* osMajorType (2 bytes) */
	Stream_Read_UINT16(s, osMinorType);             /* osMinorType (2 bytes) */
	Stream_Read_UINT16(s, protocolVersion);         /* protocolVersion (2 bytes) */
	Stream_Read_UINT16(s, pad2OctetsA);             /* pad2OctetsA (2 bytes) */
	Stream_Read_UINT16(s, generalCompressionTypes); /* generalCompressionTypes (2 bytes) */
	Stream_Read_UINT16(s, extraFlags);              /* extraFlags (2 bytes) */
	Stream_Read_UINT16(s, updateCapabilityFlag);    /* updateCapabilityFlag (2 bytes) */
	Stream_Read_UINT16(s, remoteUnshareFlag);       /* remoteUnshareFlag (2 bytes) */
	Stream_Read_UINT16(s, generalCompressionLevel); /* generalCompressionLevel (2 bytes) */
	Stream_Read_UINT8(s, refreshRectSupport);       /* refreshRectSupport (1 byte) */
	Stream_Read_UINT8(s, suppressOutputSupport);    /* suppressOutputSupport (1 byte) */
	WLog_INFO(TAG, "\tosMajorType: 0x%04" PRIX16 "", osMajorType);
	WLog_INFO(TAG, "\tosMinorType: 0x%04" PRIX16 "", osMinorType);
	WLog_INFO(TAG, "\tprotocolVersion: 0x%04" PRIX16 "", protocolVersion);
	WLog_INFO(TAG, "\tpad2OctetsA: 0x%04" PRIX16 "", pad2OctetsA);
	WLog_INFO(TAG, "\tgeneralCompressionTypes: 0x%04" PRIX16 "", generalCompressionTypes);
	WLog_INFO(TAG, "\textraFlags: 0x%04" PRIX16 "", extraFlags);
	WLog_INFO(TAG, "\tupdateCapabilityFlag: 0x%04" PRIX16 "", updateCapabilityFlag);
	WLog_INFO(TAG, "\tremoteUnshareFlag: 0x%04" PRIX16 "", remoteUnshareFlag);
	WLog_INFO(TAG, "\tgeneralCompressionLevel: 0x%04" PRIX16 "", generalCompressionLevel);
	WLog_INFO(TAG, "\trefreshRectSupport: 0x%02" PRIX8 "", refreshRectSupport);
	WLog_INFO(TAG, "\tsuppressOutputSupport: 0x%02" PRIX8 "", suppressOutputSupport);
	return TRUE;
}