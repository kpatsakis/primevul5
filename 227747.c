static BOOL rdp_write_order_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT16 orderFlags;
	UINT16 orderSupportExFlags;
	UINT16 textANSICodePage = 0;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	/* see [MSDN-CP]: http://msdn.microsoft.com/en-us/library/dd317756 */
	if (!settings->ServerMode)
		textANSICodePage = CP_UTF8; /* Unicode (UTF-8) */

	orderSupportExFlags = 0;
	orderFlags = NEGOTIATE_ORDER_SUPPORT | ZERO_BOUNDS_DELTA_SUPPORT | COLOR_INDEX_SUPPORT;

	if (settings->BitmapCacheV3Enabled)
	{
		orderSupportExFlags |= CACHE_BITMAP_V3_SUPPORT;
		orderFlags |= ORDER_FLAGS_EXTRA_SUPPORT;
	}

	if (settings->FrameMarkerCommandEnabled)
	{
		orderSupportExFlags |= ALTSEC_FRAME_MARKER_SUPPORT;
		orderFlags |= ORDER_FLAGS_EXTRA_SUPPORT;
	}

	Stream_Zero(s, 16);                          /* terminalDescriptor (16 bytes) */
	Stream_Write_UINT32(s, 0);                   /* pad4OctetsA (4 bytes) */
	Stream_Write_UINT16(s, 1);                   /* desktopSaveXGranularity (2 bytes) */
	Stream_Write_UINT16(s, 20);                  /* desktopSaveYGranularity (2 bytes) */
	Stream_Write_UINT16(s, 0);                   /* pad2OctetsA (2 bytes) */
	Stream_Write_UINT16(s, 1);                   /* maximumOrderLevel (2 bytes) */
	Stream_Write_UINT16(s, 0);                   /* numberFonts (2 bytes) */
	Stream_Write_UINT16(s, orderFlags);          /* orderFlags (2 bytes) */
	Stream_Write(s, settings->OrderSupport, 32); /* orderSupport (32 bytes) */
	Stream_Write_UINT16(s, 0);                   /* textFlags (2 bytes) */
	Stream_Write_UINT16(s, orderSupportExFlags); /* orderSupportExFlags (2 bytes) */
	Stream_Write_UINT32(s, 0);                   /* pad4OctetsB (4 bytes) */
	Stream_Write_UINT32(s, 230400);              /* desktopSaveSize (4 bytes) */
	Stream_Write_UINT16(s, 0);                   /* pad2OctetsC (2 bytes) */
	Stream_Write_UINT16(s, 0);                   /* pad2OctetsD (2 bytes) */
	Stream_Write_UINT16(s, textANSICodePage);    /* textANSICodePage (2 bytes) */
	Stream_Write_UINT16(s, 0);                   /* pad2OctetsE (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_ORDER);
	return TRUE;
}