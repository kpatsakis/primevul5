static BOOL autodetect_send_continuous_bandwidth_measure_stop(rdpContext* context,
                                                              UINT16 sequenceNumber)
{
	return autodetect_send_bandwidth_measure_stop(context, 0, sequenceNumber,
	                                              RDP_BW_STOP_REQUEST_TYPE_CONTINUOUS);
}