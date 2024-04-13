static BOOL autodetect_recv_bandwidth_measure_stop(rdpRdp* rdp, wStream* s,
                                                   AUTODETECT_REQ_PDU* autodetectReqPdu)
{
	UINT16 payloadLength;
	UINT16 responseType;

	if (autodetectReqPdu->requestType == RDP_BW_STOP_REQUEST_TYPE_CONNECTTIME)
	{
		if (autodetectReqPdu->headerLength != 0x08)
			return FALSE;

		if (Stream_GetRemainingLength(s) < 2)
			return FALSE;

		Stream_Read_UINT16(s, payloadLength); /* payloadLength (2 bytes) */
	}
	else
	{
		if (autodetectReqPdu->headerLength != 0x06)
			return FALSE;

		payloadLength = 0;
	}

	if (!Stream_SafeSeek(s, payloadLength))
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "received Bandwidth Measure Stop PDU -> payloadLength=%" PRIu16 "",
	         payloadLength);
	/* Add the payload length to the bandwidth measurement parameters */
	rdp->autodetect->bandwidthMeasureByteCount += payloadLength;

	/* Continuous Auto-Detection: mark the stop of the measurement */
	if (autodetectReqPdu->requestType == RDP_BW_STOP_REQUEST_TYPE_CONTINUOUS)
	{
		rdp->autodetect->bandwidthMeasureStarted = FALSE;
	}

	/* Send a response the server */
	responseType = autodetectReqPdu->requestType == RDP_BW_STOP_REQUEST_TYPE_CONNECTTIME
	                   ? RDP_BW_RESULTS_RESPONSE_TYPE_CONNECTTIME
	                   : RDP_BW_RESULTS_RESPONSE_TYPE_CONTINUOUS;
	return autodetect_send_bandwidth_measure_results(rdp, responseType,
	                                                 autodetectReqPdu->sequenceNumber);
}