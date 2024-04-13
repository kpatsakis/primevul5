static BOOL autodetect_recv_rtt_measure_response(rdpRdp* rdp, wStream* s,
                                                 AUTODETECT_RSP_PDU* autodetectRspPdu)
{
	BOOL success = TRUE;

	if (autodetectRspPdu->headerLength != 0x06)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "received RTT Measure Response PDU");
	rdp->autodetect->netCharAverageRTT = GetTickCount64() - rdp->autodetect->rttMeasureStartTime;

	if (rdp->autodetect->netCharBaseRTT == 0 ||
	    rdp->autodetect->netCharBaseRTT > rdp->autodetect->netCharAverageRTT)
		rdp->autodetect->netCharBaseRTT = rdp->autodetect->netCharAverageRTT;

	IFCALLRET(rdp->autodetect->RTTMeasureResponse, success, rdp->context,
	          autodetectRspPdu->sequenceNumber);
	return success;
}