static BOOL rdp_print_window_activation_capability_set(wStream* s, UINT16 length)
{
	UINT16 helpKeyFlag;
	UINT16 helpKeyIndexFlag;
	UINT16 helpExtendedKeyFlag;
	UINT16 windowManagerKeyFlag;
	WLog_INFO(TAG, "WindowActivationCapabilitySet (length %" PRIu16 "):", length);

	if (length < 12)
		return FALSE;

	Stream_Read_UINT16(s, helpKeyFlag);          /* helpKeyFlag (2 bytes) */
	Stream_Read_UINT16(s, helpKeyIndexFlag);     /* helpKeyIndexFlag (2 bytes) */
	Stream_Read_UINT16(s, helpExtendedKeyFlag);  /* helpExtendedKeyFlag (2 bytes) */
	Stream_Read_UINT16(s, windowManagerKeyFlag); /* windowManagerKeyFlag (2 bytes) */
	WLog_INFO(TAG, "\thelpKeyFlag: 0x%04" PRIX16 "", helpKeyFlag);
	WLog_INFO(TAG, "\thelpKeyIndexFlag: 0x%04" PRIX16 "", helpKeyIndexFlag);
	WLog_INFO(TAG, "\thelpExtendedKeyFlag: 0x%04" PRIX16 "", helpExtendedKeyFlag);
	WLog_INFO(TAG, "\twindowManagerKeyFlag: 0x%04" PRIX16 "", windowManagerKeyFlag);
	return TRUE;
}