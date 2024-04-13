int rdp_recv_autodetect_request_packet(rdpRdp* rdp, wStream* s)
{
	AUTODETECT_REQ_PDU autodetectReqPdu;
	BOOL success = FALSE;

	if (Stream_GetRemainingLength(s) < 6)
		return -1;

	Stream_Read_UINT8(s, autodetectReqPdu.headerLength);    /* headerLength (1 byte) */
	Stream_Read_UINT8(s, autodetectReqPdu.headerTypeId);    /* headerTypeId (1 byte) */
	Stream_Read_UINT16(s, autodetectReqPdu.sequenceNumber); /* sequenceNumber (2 bytes) */
	Stream_Read_UINT16(s, autodetectReqPdu.requestType);    /* requestType (2 bytes) */
	WLog_VRB(AUTODETECT_TAG,
	         "rdp_recv_autodetect_request_packet: headerLength=%" PRIu8 ", headerTypeId=%" PRIu8
	         ", sequenceNumber=%" PRIu16 ", requestType=%04" PRIx16 "",
	         autodetectReqPdu.headerLength, autodetectReqPdu.headerTypeId,
	         autodetectReqPdu.sequenceNumber, autodetectReqPdu.requestType);

	if (autodetectReqPdu.headerTypeId != TYPE_ID_AUTODETECT_REQUEST)
		return -1;

	switch (autodetectReqPdu.requestType)
	{
		case RDP_RTT_REQUEST_TYPE_CONTINUOUS:
		case RDP_RTT_REQUEST_TYPE_CONNECTTIME:
			/* RTT Measure Request (RDP_RTT_REQUEST) - MS-RDPBCGR 2.2.14.1.1 */
			success = autodetect_recv_rtt_measure_request(rdp, s, &autodetectReqPdu);
			break;

		case RDP_BW_START_REQUEST_TYPE_CONTINUOUS:
		case RDP_BW_START_REQUEST_TYPE_TUNNEL:
		case RDP_BW_START_REQUEST_TYPE_CONNECTTIME:
			/* Bandwidth Measure Start (RDP_BW_START) - MS-RDPBCGR 2.2.14.1.2 */
			success = autodetect_recv_bandwidth_measure_start(rdp, s, &autodetectReqPdu);
			break;

		case RDP_BW_PAYLOAD_REQUEST_TYPE:
			/* Bandwidth Measure Payload (RDP_BW_PAYLOAD) - MS-RDPBCGR 2.2.14.1.3 */
			success = autodetect_recv_bandwidth_measure_payload(rdp, s, &autodetectReqPdu);
			break;

		case RDP_BW_STOP_REQUEST_TYPE_CONNECTTIME:
		case RDP_BW_STOP_REQUEST_TYPE_CONTINUOUS:
		case RDP_BW_STOP_REQUEST_TYPE_TUNNEL:
			/* Bandwidth Measure Stop (RDP_BW_STOP) - MS-RDPBCGR 2.2.14.1.4 */
			success = autodetect_recv_bandwidth_measure_stop(rdp, s, &autodetectReqPdu);
			break;

		case 0x0840:
		case 0x0880:
		case 0x08C0:
			/* Network Characteristics Result (RDP_NETCHAR_RESULT) - MS-RDPBCGR 2.2.14.1.5 */
			success = autodetect_recv_netchar_result(rdp, s, &autodetectReqPdu);
			break;

		default:
			break;
	}

	return success ? 0 : -1;
}