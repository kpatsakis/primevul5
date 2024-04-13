INLINE void gdi_RgnToCRect(HGDI_RGN rgn, INT32* left, INT32* top, INT32* right, INT32* bottom)
{
	*left = rgn->x;
	*top = rgn->y;
	*right = rgn->x + rgn->w - 1;
	*bottom = rgn->y + rgn->h - 1;
}