INLINE BOOL gdi_SetRectRgn(HGDI_RGN hRgn, INT32 nLeftRect, INT32 nTopRect, INT32 nRightRect,
                           INT32 nBottomRect)
{
	gdi_CRectToRgn(nLeftRect, nTopRect, nRightRect, nBottomRect, hRgn);
	hRgn->null = FALSE;
	return TRUE;
}