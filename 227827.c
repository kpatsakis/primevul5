static BOOL rdp_write_nsc_client_capability_container(wStream* s, const rdpSettings* settings)
{
	BYTE colorLossLevel;
	BYTE fAllowSubsampling;
	BYTE fAllowDynamicFidelity;
	fAllowDynamicFidelity = settings->NSCodecAllowDynamicColorFidelity;
	fAllowSubsampling = settings->NSCodecAllowSubsampling;
	colorLossLevel = settings->NSCodecColorLossLevel;

	if (colorLossLevel < 1)
		colorLossLevel = 1;

	if (colorLossLevel > 7)
		colorLossLevel = 7;

	if (!Stream_EnsureRemainingCapacity(s, 8))
		return FALSE;

	Stream_Write_UINT16(s, 3); /* codecPropertiesLength */
	/* TS_NSCODEC_CAPABILITYSET */
	Stream_Write_UINT8(s, fAllowDynamicFidelity); /* fAllowDynamicFidelity (1 byte) */
	Stream_Write_UINT8(s, fAllowSubsampling);     /* fAllowSubsampling (1 byte) */
	Stream_Write_UINT8(s, colorLossLevel);        /* colorLossLevel (1 byte) */
	return TRUE;
}