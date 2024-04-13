HGDI_RGN gdi_CreateRectRgn(INT32 nLeftRect, INT32 nTopRect, INT32 nRightRect, INT32 nBottomRect)
{
	HGDI_RGN hRgn = (HGDI_RGN)calloc(1, sizeof(GDI_RGN));

	if (!hRgn)
		return NULL;

	hRgn->objectType = GDIOBJECT_REGION;
	hRgn->x = nLeftRect;
	hRgn->y = nTopRect;
	hRgn->w = nRightRect - nLeftRect + 1;
	hRgn->h = nBottomRect - nTopRect + 1;
	hRgn->null = FALSE;
	return hRgn;
}