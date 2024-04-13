void autodetect_register_server_callbacks(rdpAutoDetect* autodetect)
{
	autodetect->RTTMeasureRequest = autodetect_send_continuous_rtt_measure_request;
	autodetect->BandwidthMeasureStart = autodetect_send_continuous_bandwidth_measure_start;
	autodetect->BandwidthMeasureStop = autodetect_send_continuous_bandwidth_measure_stop;
	autodetect->NetworkCharacteristicsResult = autodetect_send_netchar_result;
}