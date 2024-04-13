static BOOL autodetect_send_rtt_measure_response(rdpRdp* rdp, UINT16 sequenceNumber)
{
	wStream* s;
	/* Send the response PDU to the server */
	s = rdp_message_channel_pdu_init(rdp);

	if (!s)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "sending RTT Measure Response PDU");
	Stream_Write_UINT8(s, 0x06);                        /* headerLength (1 byte) */
	Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_RESPONSE); /* headerTypeId (1 byte) */
	Stream_Write_UINT16(s, sequenceNumber);             /* sequenceNumber (2 bytes) */
	Stream_Write_UINT16(s, RDP_RTT_RESPONSE_TYPE);      /* responseType (1 byte) */
	return rdp_send_message_channel_pdu(rdp, s, SEC_AUTODETECT_RSP);
}