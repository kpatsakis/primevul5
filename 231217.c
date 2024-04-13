mwifiex_band_to_radio_type(u8 band)
{
	switch (band) {
	case BAND_A:
	case BAND_AN:
	case BAND_A | BAND_AN:
	case BAND_A | BAND_AN | BAND_AAC:
		return HostCmd_SCAN_RADIO_TYPE_A;
	case BAND_B:
	case BAND_G:
	case BAND_B | BAND_G:
	default:
		return HostCmd_SCAN_RADIO_TYPE_BG;
	}
}