BOOL rdp_recv_demand_active(rdpRdp* rdp, wStream* s)
{
	UINT16 channelId;
	UINT16 pduType;
	UINT16 pduLength;
	UINT16 pduSource;
	UINT16 length;
	UINT16 numberCapabilities;
	UINT16 lengthSourceDescriptor;
	UINT16 lengthCombinedCapabilities;

	if (!rdp_recv_get_active_header(rdp, s, &channelId, &length))
		return FALSE;

	if (freerdp_shall_disconnect(rdp->instance))
		return TRUE;

	if (!rdp_read_share_control_header(s, &pduLength, &pduType, &pduSource))
	{
		WLog_ERR(TAG, "rdp_read_share_control_header failed");
		return FALSE;
	}

	if (pduType == PDU_TYPE_DATA)
	{
		/**
		 * We can receive a Save Session Info Data PDU containing a LogonErrorInfo
		 * structure at this point from the server to indicate a connection error.
		 */
		if (rdp_recv_data_pdu(rdp, s) < 0)
			return FALSE;

		return FALSE;
	}

	if (pduType != PDU_TYPE_DEMAND_ACTIVE)
	{
		if (pduType != PDU_TYPE_SERVER_REDIRECTION)
			WLog_ERR(TAG, "expected PDU_TYPE_DEMAND_ACTIVE %04x, got %04" PRIx16 "",
			         PDU_TYPE_DEMAND_ACTIVE, pduType);

		return FALSE;
	}

	rdp->settings->PduSource = pduSource;

	if (Stream_GetRemainingLength(s) < 8)
		return FALSE;

	Stream_Read_UINT32(s, rdp->settings->ShareId);     /* shareId (4 bytes) */
	Stream_Read_UINT16(s, lengthSourceDescriptor);     /* lengthSourceDescriptor (2 bytes) */
	Stream_Read_UINT16(s, lengthCombinedCapabilities); /* lengthCombinedCapabilities (2 bytes) */

	if (!Stream_SafeSeek(s, lengthSourceDescriptor) ||
	    Stream_GetRemainingLength(s) < 4) /* sourceDescriptor */
		return FALSE;

	Stream_Read_UINT16(s, numberCapabilities); /* numberCapabilities (2 bytes) */
	Stream_Seek(s, 2);                         /* pad2Octets (2 bytes) */

	/* capabilitySets */
	if (!rdp_read_capability_sets(s, rdp->settings, numberCapabilities, lengthCombinedCapabilities))
	{
		WLog_ERR(TAG, "rdp_read_capability_sets failed");
		return FALSE;
	}

	if (!Stream_SafeSeek(s, 4)) /* SessionId */
		return FALSE;

	rdp->update->secondary->glyph_v2 = (rdp->settings->GlyphSupportLevel > GLYPH_SUPPORT_FULL);
	return tpkt_ensure_stream_consumed(s, length);
}