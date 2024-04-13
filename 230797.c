static BOOL autodetect_send_netchar_result(rdpContext* context, UINT16 sequenceNumber)
{
	wStream* s;
	s = rdp_message_channel_pdu_init(context->rdp);

	if (!s)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "sending Bandwidth Network Characteristics Result PDU");

	if (context->rdp->autodetect->netCharBandwidth > 0)
	{
		Stream_Write_UINT8(s, 0x12);                       /* headerLength (1 byte) */
		Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_REQUEST); /* headerTypeId (1 byte) */
		Stream_Write_UINT16(s, sequenceNumber);            /* sequenceNumber (2 bytes) */
		Stream_Write_UINT16(s, 0x08C0);                    /* requestType (2 bytes) */
		Stream_Write_UINT32(s, context->rdp->autodetect->netCharBaseRTT); /* baseRTT (4 bytes) */
		Stream_Write_UINT32(s,
		                    context->rdp->autodetect->netCharBandwidth); /* bandwidth (4 bytes) */
		Stream_Write_UINT32(s,
		                    context->rdp->autodetect->netCharAverageRTT); /* averageRTT (4 bytes) */
	}
	else
	{
		Stream_Write_UINT8(s, 0x0E);                       /* headerLength (1 byte) */
		Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_REQUEST); /* headerTypeId (1 byte) */
		Stream_Write_UINT16(s, sequenceNumber);            /* sequenceNumber (2 bytes) */
		Stream_Write_UINT16(s, 0x0840);                    /* requestType (2 bytes) */
		Stream_Write_UINT32(s, context->rdp->autodetect->netCharBaseRTT); /* baseRTT (4 bytes) */
		Stream_Write_UINT32(s,
		                    context->rdp->autodetect->netCharAverageRTT); /* averageRTT (4 bytes) */
	}

	return rdp_send_message_channel_pdu(context->rdp, s, SEC_AUTODETECT_REQ);
}