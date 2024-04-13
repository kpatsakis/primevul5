BOOL autodetect_send_connecttime_bandwidth_measure_stop(rdpContext* context, UINT16 payloadLength,
                                                        UINT16 sequenceNumber)
{
	return autodetect_send_bandwidth_measure_stop(context, payloadLength, sequenceNumber,
	                                              RDP_BW_STOP_REQUEST_TYPE_CONNECTTIME);
}