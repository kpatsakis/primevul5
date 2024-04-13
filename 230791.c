int rdp_recv_autodetect_response_packet(rdpRdp* rdp, wStream* s)
{
	AUTODETECT_RSP_PDU autodetectRspPdu;
	BOOL success = FALSE;

	if (Stream_GetRemainingLength(s) < 6)
		return -1;

	Stream_Read_UINT8(s, autodetectRspPdu.headerLength);    /* headerLength (1 byte) */
	Stream_Read_UINT8(s, autodetectRspPdu.headerTypeId);    /* headerTypeId (1 byte) */
	Stream_Read_UINT16(s, autodetectRspPdu.sequenceNumber); /* sequenceNumber (2 bytes) */
	Stream_Read_UINT16(s, autodetectRspPdu.responseType);   /* responseType (2 bytes) */
	WLog_VRB(AUTODETECT_TAG,
	         "rdp_recv_autodetect_response_packet: headerLength=%" PRIu8 ", headerTypeId=%" PRIu8
	         ", sequenceNumber=%" PRIu16 ", requestType=%04" PRIx16 "",
	         autodetectRspPdu.headerLength, autodetectRspPdu.headerTypeId,
	         autodetectRspPdu.sequenceNumber, autodetectRspPdu.responseType);

	if (autodetectRspPdu.headerTypeId != TYPE_ID_AUTODETECT_RESPONSE)
		return -1;

	switch (autodetectRspPdu.responseType)
	{
		case RDP_RTT_RESPONSE_TYPE:
			/* RTT Measure Response (RDP_RTT_RESPONSE) - MS-RDPBCGR 2.2.14.2.1 */
			success = autodetect_recv_rtt_measure_response(rdp, s, &autodetectRspPdu);
			break;

		case RDP_BW_RESULTS_RESPONSE_TYPE_CONNECTTIME:
		case RDP_BW_RESULTS_RESPONSE_TYPE_CONTINUOUS:
			/* Bandwidth Measure Results (RDP_BW_RESULTS) - MS-RDPBCGR 2.2.14.2.2 */
			success = autodetect_recv_bandwidth_measure_results(rdp, s, &autodetectRspPdu);
			break;

		default:
			break;
	}
	return success ? 0 : -1;
}