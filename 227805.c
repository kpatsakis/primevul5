static BOOL rdp_print_share_capability_set(wStream* s, UINT16 length)
{
	UINT16 nodeId;
	UINT16 pad2Octets;
	WLog_INFO(TAG, "ShareCapabilitySet (length %" PRIu16 "):", length);

	if (length < 8)
		return FALSE;

	Stream_Read_UINT16(s, nodeId);     /* nodeId (2 bytes) */
	Stream_Read_UINT16(s, pad2Octets); /* pad2Octets (2 bytes) */
	WLog_INFO(TAG, "\tnodeId: 0x%04" PRIX16 "", nodeId);
	WLog_INFO(TAG, "\tpad2Octets: 0x%04" PRIX16 "", pad2Octets);
	return TRUE;
}