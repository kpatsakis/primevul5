static BOOL rdp_read_bitmap_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	BYTE drawingFlags;
	UINT16 desktopWidth;
	UINT16 desktopHeight;
	UINT16 desktopResizeFlag;
	UINT16 preferredBitsPerPixel;

	if (length < 28)
		return FALSE;

	Stream_Read_UINT16(s, preferredBitsPerPixel); /* preferredBitsPerPixel (2 bytes) */
	Stream_Seek_UINT16(s);                        /* receive1BitPerPixel (2 bytes) */
	Stream_Seek_UINT16(s);                        /* receive4BitsPerPixel (2 bytes) */
	Stream_Seek_UINT16(s);                        /* receive8BitsPerPixel (2 bytes) */
	Stream_Read_UINT16(s, desktopWidth);          /* desktopWidth (2 bytes) */
	Stream_Read_UINT16(s, desktopHeight);         /* desktopHeight (2 bytes) */
	Stream_Seek_UINT16(s);                        /* pad2Octets (2 bytes) */
	Stream_Read_UINT16(s, desktopResizeFlag);     /* desktopResizeFlag (2 bytes) */
	Stream_Seek_UINT16(s);                        /* bitmapCompressionFlag (2 bytes) */
	Stream_Seek_UINT8(s);                         /* highColorFlags (1 byte) */
	Stream_Read_UINT8(s, drawingFlags);           /* drawingFlags (1 byte) */
	Stream_Seek_UINT16(s);                        /* multipleRectangleSupport (2 bytes) */
	Stream_Seek_UINT16(s);                        /* pad2OctetsB (2 bytes) */

	if (!settings->ServerMode && (preferredBitsPerPixel != settings->ColorDepth))
	{
		/* The client must respect the actual color depth used by the server */
		settings->ColorDepth = preferredBitsPerPixel;
	}

	if (desktopResizeFlag == FALSE)
		settings->DesktopResize = FALSE;

	if (!settings->ServerMode && settings->DesktopResize)
	{
		/* The server may request a different desktop size during Deactivation-Reactivation sequence
		 */
		settings->DesktopWidth = desktopWidth;
		settings->DesktopHeight = desktopHeight;
	}

	if (settings->DrawAllowSkipAlpha)
		settings->DrawAllowSkipAlpha = (drawingFlags & DRAW_ALLOW_SKIP_ALPHA) ? TRUE : FALSE;

	if (settings->DrawAllowDynamicColorFidelity)
		settings->DrawAllowDynamicColorFidelity =
		    (drawingFlags & DRAW_ALLOW_DYNAMIC_COLOR_FIDELITY) ? TRUE : FALSE;

	if (settings->DrawAllowColorSubsampling)
		settings->DrawAllowColorSubsampling =
		    (drawingFlags & DRAW_ALLOW_COLOR_SUBSAMPLING) ? TRUE : FALSE;

	return TRUE;
}