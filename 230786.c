static BOOL autodetect_send_bandwidth_measure_results(rdpRdp* rdp, UINT16 responseType,
                                                      UINT16 sequenceNumber)
{
	BOOL success = TRUE;
	wStream* s;
	UINT64 timeDelta;
	/* Compute the total time */
	timeDelta = GetTickCount64() - rdp->autodetect->bandwidthMeasureStartTime;
	/* Send the result PDU to the server */
	s = rdp_message_channel_pdu_init(rdp);

	if (!s)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG,
	         "sending Bandwidth Measure Results PDU -> timeDelta=%" PRIu32 ", byteCount=%" PRIu32
	         "",
	         timeDelta, rdp->autodetect->bandwidthMeasureByteCount);
	Stream_Write_UINT8(s, 0x0E);                        /* headerLength (1 byte) */
	Stream_Write_UINT8(s, TYPE_ID_AUTODETECT_RESPONSE); /* headerTypeId (1 byte) */
	Stream_Write_UINT16(s, sequenceNumber);             /* sequenceNumber (2 bytes) */
	Stream_Write_UINT16(s, responseType);               /* responseType (1 byte) */
	Stream_Write_UINT32(s, timeDelta);                  /* timeDelta (4 bytes) */
	Stream_Write_UINT32(s, rdp->autodetect->bandwidthMeasureByteCount); /* byteCount (4 bytes) */
	IFCALLRET(rdp->autodetect->ClientBandwidthMeasureResult, success, rdp->context,
	          rdp->autodetect);

	if (!success)
		return FALSE;

	return rdp_send_message_channel_pdu(rdp, s, SEC_AUTODETECT_RSP);
}