static BOOL rdp_write_bitmap_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	BYTE drawingFlags = 0;
	UINT16 preferredBitsPerPixel;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	if (settings->DrawAllowSkipAlpha)
		drawingFlags |= DRAW_ALLOW_SKIP_ALPHA;

	if (settings->DrawAllowDynamicColorFidelity)
		drawingFlags |= DRAW_ALLOW_DYNAMIC_COLOR_FIDELITY;

	if (settings->DrawAllowColorSubsampling)
		drawingFlags |= DRAW_ALLOW_COLOR_SUBSAMPLING; /* currently unimplemented */

	/* While bitmap_decode.c now implements YCoCg, in turning it
	 * on we have found Microsoft is inconsistent on whether to invert R & B.
	 * And it's not only from one server to another; on Win7/2008R2, it appears
	 * to send the main content with a different inversion than the Windows
	 * button!  So... don't advertise that we support YCoCg and the server
	 * will not send it.  YCoCg is still needed for EGFX, but it at least
	 * appears consistent in its use.
	 */

	if ((settings->ColorDepth > UINT16_MAX) || (settings->DesktopWidth > UINT16_MAX) ||
	    (settings->DesktopHeight > UINT16_MAX) || (settings->DesktopResize > UINT16_MAX))
		return FALSE;

	if (settings->RdpVersion >= RDP_VERSION_5_PLUS)
		preferredBitsPerPixel = (UINT16)settings->ColorDepth;
	else
		preferredBitsPerPixel = 8;

	Stream_Write_UINT16(s, preferredBitsPerPixel);   /* preferredBitsPerPixel (2 bytes) */
	Stream_Write_UINT16(s, 1);                       /* receive1BitPerPixel (2 bytes) */
	Stream_Write_UINT16(s, 1);                       /* receive4BitsPerPixel (2 bytes) */
	Stream_Write_UINT16(s, 1);                       /* receive8BitsPerPixel (2 bytes) */
	Stream_Write_UINT16(s, (UINT16)settings->DesktopWidth);  /* desktopWidth (2 bytes) */
	Stream_Write_UINT16(s, (UINT16)settings->DesktopHeight); /* desktopHeight (2 bytes) */
	Stream_Write_UINT16(s, 0);                       /* pad2Octets (2 bytes) */
	Stream_Write_UINT16(s, (UINT16)settings->DesktopResize); /* desktopResizeFlag (2 bytes) */
	Stream_Write_UINT16(s, 1);                       /* bitmapCompressionFlag (2 bytes) */
	Stream_Write_UINT8(s, 0);                        /* highColorFlags (1 byte) */
	Stream_Write_UINT8(s, drawingFlags);             /* drawingFlags (1 byte) */
	Stream_Write_UINT16(s, 1);                       /* multipleRectangleSupport (2 bytes) */
	Stream_Write_UINT16(s, 0);                       /* pad2OctetsB (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_BITMAP);
	return TRUE;
}