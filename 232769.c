INLINE BOOL gdi_EqualRgn(HGDI_RGN hSrcRgn1, HGDI_RGN hSrcRgn2)
{
	if ((hSrcRgn1->x == hSrcRgn2->x) && (hSrcRgn1->y == hSrcRgn2->y) &&
	    (hSrcRgn1->w == hSrcRgn2->w) && (hSrcRgn1->h == hSrcRgn2->h))
	{
		return TRUE;
	}

	return FALSE;
}