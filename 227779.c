static BOOL rdp_read_input_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	UINT16 inputFlags;

	if (length < 88)
		return FALSE;

	Stream_Read_UINT16(s, inputFlags); /* inputFlags (2 bytes) */
	Stream_Seek_UINT16(s);             /* pad2OctetsA (2 bytes) */

	if (settings->ServerMode)
	{
		Stream_Read_UINT32(s, settings->KeyboardLayout);      /* keyboardLayout (4 bytes) */
		Stream_Read_UINT32(s, settings->KeyboardType);        /* keyboardType (4 bytes) */
		Stream_Read_UINT32(s, settings->KeyboardSubType);     /* keyboardSubType (4 bytes) */
		Stream_Read_UINT32(s, settings->KeyboardFunctionKey); /* keyboardFunctionKeys (4 bytes) */
	}
	else
	{
		Stream_Seek_UINT32(s); /* keyboardLayout (4 bytes) */
		Stream_Seek_UINT32(s); /* keyboardType (4 bytes) */
		Stream_Seek_UINT32(s); /* keyboardSubType (4 bytes) */
		Stream_Seek_UINT32(s); /* keyboardFunctionKeys (4 bytes) */
	}

	Stream_Seek(s, 64); /* imeFileName (64 bytes) */

	if (!settings->ServerMode)
	{
		if (inputFlags & INPUT_FLAG_FASTPATH_INPUT)
		{
			/* advertised by RDP 5.0 and 5.1 servers */
		}
		else if (inputFlags & INPUT_FLAG_FASTPATH_INPUT2)
		{
			/* advertised by RDP 5.2, 6.0, 6.1 and 7.0 servers */
		}
		else
		{
			/* server does not support fastpath input */
			settings->FastPathInput = FALSE;
		}

		if (inputFlags & TS_INPUT_FLAG_MOUSE_HWHEEL)
			settings->HasHorizontalWheel = TRUE;

		if (inputFlags & INPUT_FLAG_UNICODE)
			settings->UnicodeInput = TRUE;

		if (inputFlags & INPUT_FLAG_MOUSEX)
			settings->HasExtendedMouseEvent = TRUE;
	}

	return TRUE;
}