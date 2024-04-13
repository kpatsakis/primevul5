BOOL autodetect_send_bandwidth_measure_payload(rdpContext* context, UINT16 payloadLength,
                                               UINT16 sequenceNumber)
{
	wStream* s;
	UCHAR* buffer = NULL;
	BOOL bResult = FALSE;
	s = rdp_message_channel_pdu_init(context->rdp);

	if (!s)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "sending Bandwidth Measure Payload PDU -> payloadLength=%" PRIu16 "",
	         payloadLength);
	/* 4-bytes aligned */
	payloadLength &= ~3;

	if (!Stream_EnsureRemainingCapacity(s, 8 + payloadLength))
	{
		Stream_Release(s);
		return FALSE;
	}

	Stream_Write_UINT8(s, 0x08);                         /* headerLength (1 byte) */
	Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_REQUEST);   /* headerTypeId (1 byte) */
	Stream_Write_UINT16(s, sequenceNumber);              /* sequenceNumber (2 bytes) */
	Stream_Write_UINT16(s, RDP_BW_PAYLOAD_REQUEST_TYPE); /* requestType (2 bytes) */
	Stream_Write_UINT16(s, payloadLength);               /* payloadLength (2 bytes) */
	/* Random data (better measurement in case the line is compressed) */
	buffer = (UCHAR*)malloc(payloadLength);

	if (NULL == buffer)
	{
		Stream_Release(s);
		return FALSE;
	}

	winpr_RAND(buffer, payloadLength);
	Stream_Write(s, buffer, payloadLength);
	bResult = rdp_send_message_channel_pdu(context->rdp, s, SEC_AUTODETECT_REQ);
	free(buffer);
	return bResult;
}