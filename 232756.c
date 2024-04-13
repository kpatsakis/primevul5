INLINE BOOL gdi_InvalidateRegion(HGDI_DC hdc, INT32 x, INT32 y, INT32 w, INT32 h)
{
	GDI_RECT inv;
	GDI_RECT rgn;
	HGDI_RGN invalid;
	HGDI_RGN cinvalid;

	if (!hdc->hwnd)
		return TRUE;

	if (!hdc->hwnd->invalid)
		return TRUE;

	if (w == 0 || h == 0)
		return TRUE;

	cinvalid = hdc->hwnd->cinvalid;

	if ((hdc->hwnd->ninvalid + 1) > (INT64)hdc->hwnd->count)
	{
		int new_cnt;
		HGDI_RGN new_rgn;
		new_cnt = hdc->hwnd->count * 2;
		new_rgn = (HGDI_RGN)realloc(cinvalid, sizeof(GDI_RGN) * new_cnt);

		if (!new_rgn)
			return FALSE;

		hdc->hwnd->count = new_cnt;
		cinvalid = new_rgn;
	}

	gdi_SetRgn(&cinvalid[hdc->hwnd->ninvalid++], x, y, w, h);
	hdc->hwnd->cinvalid = cinvalid;
	invalid = hdc->hwnd->invalid;

	if (invalid->null)
	{
		invalid->x = x;
		invalid->y = y;
		invalid->w = w;
		invalid->h = h;
		invalid->null = FALSE;
		return TRUE;
	}

	gdi_CRgnToRect(x, y, w, h, &rgn);
	gdi_RgnToRect(invalid, &inv);

	if (rgn.left < inv.left)
		inv.left = rgn.left;

	if (rgn.top < inv.top)
		inv.top = rgn.top;

	if (rgn.right > inv.right)
		inv.right = rgn.right;

	if (rgn.bottom > inv.bottom)
		inv.bottom = rgn.bottom;

	gdi_RectToRgn(&inv, invalid);
	return TRUE;
}