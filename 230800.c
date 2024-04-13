static BOOL autodetect_recv_bandwidth_measure_payload(rdpRdp* rdp, wStream* s,
                                                      AUTODETECT_REQ_PDU* autodetectReqPdu)
{
	UINT16 payloadLength;

	if (autodetectReqPdu->headerLength != 0x08)
		return FALSE;

	if (Stream_GetRemainingLength(s) < 2)
		return FALSE;

	Stream_Read_UINT16(s, payloadLength); /* payloadLength (2 bytes) */
	if (!Stream_SafeSeek(s, payloadLength))
		return FALSE;
	WLog_DBG(AUTODETECT_TAG, "received Bandwidth Measure Payload PDU -> payloadLength=%" PRIu16 "",
	         payloadLength);
	/* Add the payload length to the bandwidth measurement parameters */
	rdp->autodetect->bandwidthMeasureByteCount += payloadLength;
	return TRUE;
}