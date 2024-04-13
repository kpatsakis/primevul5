static BOOL autodetect_recv_rtt_measure_request(rdpRdp* rdp, wStream* s,
                                                AUTODETECT_REQ_PDU* autodetectReqPdu)
{
	if (autodetectReqPdu->headerLength != 0x06)
		return FALSE;

	WLog_VRB(AUTODETECT_TAG, "received RTT Measure Request PDU");
	/* Send a response to the server */
	return autodetect_send_rtt_measure_response(rdp, autodetectReqPdu->sequenceNumber);
}