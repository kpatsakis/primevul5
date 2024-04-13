BOOL license_read_error_alert_packet(rdpLicense* license, wStream* s)
{
	UINT32 dwErrorCode;
	UINT32 dwStateTransition;

	if (Stream_GetRemainingLength(s) < 8)
		return FALSE;

	Stream_Read_UINT32(s, dwErrorCode);       /* dwErrorCode (4 bytes) */
	Stream_Read_UINT32(s, dwStateTransition); /* dwStateTransition (4 bytes) */

	if (!license_read_binary_blob(s, license->ErrorInfo)) /* bbErrorInfo */
		return FALSE;

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "dwErrorCode: %s, dwStateTransition: %s", error_codes[dwErrorCode],
	         state_transitions[dwStateTransition]);
#endif

	if (dwErrorCode == STATUS_VALID_CLIENT)
	{
		license->state = LICENSE_STATE_COMPLETED;
		return TRUE;
	}

	switch (dwStateTransition)
	{
		case ST_TOTAL_ABORT:
			license->state = LICENSE_STATE_ABORTED;
			break;
		case ST_NO_TRANSITION:
			license->state = LICENSE_STATE_COMPLETED;
			break;
		case ST_RESET_PHASE_TO_START:
			license->state = LICENSE_STATE_AWAIT;
			break;
		case ST_RESEND_LAST_MESSAGE:
			break;
		default:
			break;
	}

	return TRUE;
}