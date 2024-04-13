INLINE void gdi_CRectToRgn(INT32 left, INT32 top, INT32 right, INT32 bottom, HGDI_RGN rgn)
{
	rgn->x = left;
	rgn->y = top;
	rgn->w = right - left + 1;
	rgn->h = bottom - top + 1;
}