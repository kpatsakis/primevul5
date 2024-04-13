static BOOL autodetect_send_rtt_measure_request(rdpContext* context, UINT16 sequenceNumber,
                                                UINT16 requestType)
{
	wStream* s;
	s = rdp_message_channel_pdu_init(context->rdp);

	if (!s)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "sending RTT Measure Request PDU");
	Stream_Write_UINT8(s, 0x06);                       /* headerLength (1 byte) */
	Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_REQUEST); /* headerTypeId (1 byte) */
	Stream_Write_UINT16(s, sequenceNumber);            /* sequenceNumber (2 bytes) */
	Stream_Write_UINT16(s, requestType);               /* requestType (2 bytes) */
	context->rdp->autodetect->rttMeasureStartTime = GetTickCount64();
	return rdp_send_message_channel_pdu(context->rdp, s, SEC_AUTODETECT_REQ);
}