INLINE BOOL gdi_SetRgn(HGDI_RGN hRgn, INT32 nXLeft, INT32 nYLeft, INT32 nWidth, INT32 nHeight)
{
	hRgn->x = nXLeft;
	hRgn->y = nYLeft;
	hRgn->w = nWidth;
	hRgn->h = nHeight;
	hRgn->null = FALSE;
	return TRUE;
}