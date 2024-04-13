static BOOL autodetect_recv_bandwidth_measure_start(rdpRdp* rdp, wStream* s,
                                                    AUTODETECT_REQ_PDU* autodetectReqPdu)
{
	if (autodetectReqPdu->headerLength != 0x06)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "received Bandwidth Measure Start PDU - time=%" PRIu64 "",
	         GetTickCount64());
	/* Initialize bandwidth measurement parameters */
	rdp->autodetect->bandwidthMeasureStartTime = GetTickCount64();
	rdp->autodetect->bandwidthMeasureByteCount = 0;

	/* Continuous Auto-Detection: mark the start of the measurement */
	if (autodetectReqPdu->requestType == RDP_BW_START_REQUEST_TYPE_CONTINUOUS)
	{
		rdp->autodetect->bandwidthMeasureStarted = TRUE;
	}

	return TRUE;
}