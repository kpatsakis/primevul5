static BOOL autodetect_send_bandwidth_measure_start(rdpContext* context, UINT16 sequenceNumber,
                                                    UINT16 requestType)
{
	wStream* s;
	s = rdp_message_channel_pdu_init(context->rdp);

	if (!s)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "sending Bandwidth Measure Start PDU");
	Stream_Write_UINT8(s, 0x06);                       /* headerLength (1 byte) */
	Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_REQUEST); /* headerTypeId (1 byte) */
	Stream_Write_UINT16(s, sequenceNumber);            /* sequenceNumber (2 bytes) */
	Stream_Write_UINT16(s, requestType);               /* requestType (2 bytes) */
	return rdp_send_message_channel_pdu(context->rdp, s, SEC_AUTODETECT_REQ);
}