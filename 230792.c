static BOOL autodetect_send_continuous_rtt_measure_request(rdpContext* context,
                                                           UINT16 sequenceNumber)
{
	return autodetect_send_rtt_measure_request(context, sequenceNumber,
	                                           RDP_RTT_REQUEST_TYPE_CONTINUOUS);
}