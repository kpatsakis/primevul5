static BOOL license_send(rdpLicense* license, wStream* s, BYTE type)
{
	size_t length;
	BYTE flags;
	UINT16 wMsgSize;
	rdpRdp* rdp = license->rdp;
	BOOL ret;

	DEBUG_LICENSE("Sending %s Packet", LICENSE_MESSAGE_STRINGS[type & 0x1F]);
	length = Stream_GetPosition(s);
	wMsgSize = length - license->PacketHeaderLength;
	Stream_SetPosition(s, license->PacketHeaderLength);
	flags = PREAMBLE_VERSION_3_0;

	/**
	 * Using EXTENDED_ERROR_MSG_SUPPORTED here would cause mstsc to crash when
	 * running in server mode! This flag seems to be incorrectly documented.
	 */

	if (!rdp->settings->ServerMode)
		flags |= EXTENDED_ERROR_MSG_SUPPORTED;

	if (!license_write_preamble(s, type, flags, wMsgSize))
		return FALSE;

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "Sending %s Packet, length %" PRIu16 "", LICENSE_MESSAGE_STRINGS[type & 0x1F],
	         wMsgSize);
	winpr_HexDump(TAG, WLOG_DEBUG, Stream_Pointer(s) - LICENSE_PREAMBLE_LENGTH, wMsgSize);
#endif
	Stream_SetPosition(s, length);
	ret = rdp_send(rdp, s, MCS_GLOBAL_CHANNEL_ID);
	rdp->sec_flags = 0;
	return ret;
}