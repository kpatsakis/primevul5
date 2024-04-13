static BOOL rdp_print_input_capability_set(wStream* s, UINT16 length)
{
	UINT16 inputFlags;
	UINT16 pad2OctetsA;
	UINT32 keyboardLayout;
	UINT32 keyboardType;
	UINT32 keyboardSubType;
	UINT32 keyboardFunctionKey;
	WLog_INFO(TAG, "InputCapabilitySet (length %" PRIu16 ")", length);

	if (length < 88)
		return FALSE;

	Stream_Read_UINT16(s, inputFlags);          /* inputFlags (2 bytes) */
	Stream_Read_UINT16(s, pad2OctetsA);         /* pad2OctetsA (2 bytes) */
	Stream_Read_UINT32(s, keyboardLayout);      /* keyboardLayout (4 bytes) */
	Stream_Read_UINT32(s, keyboardType);        /* keyboardType (4 bytes) */
	Stream_Read_UINT32(s, keyboardSubType);     /* keyboardSubType (4 bytes) */
	Stream_Read_UINT32(s, keyboardFunctionKey); /* keyboardFunctionKeys (4 bytes) */
	Stream_Seek(s, 64);                         /* imeFileName (64 bytes) */
	WLog_INFO(TAG, "\tinputFlags: 0x%04" PRIX16 "", inputFlags);
	WLog_INFO(TAG, "\tpad2OctetsA: 0x%04" PRIX16 "", pad2OctetsA);
	WLog_INFO(TAG, "\tkeyboardLayout: 0x%08" PRIX32 "", keyboardLayout);
	WLog_INFO(TAG, "\tkeyboardType: 0x%08" PRIX32 "", keyboardType);
	WLog_INFO(TAG, "\tkeyboardSubType: 0x%08" PRIX32 "", keyboardSubType);
	WLog_INFO(TAG, "\tkeyboardFunctionKey: 0x%08" PRIX32 "", keyboardFunctionKey);
	return TRUE;
}