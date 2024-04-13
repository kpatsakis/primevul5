HGDI_RECT gdi_CreateRect(INT32 xLeft, INT32 yTop, INT32 xRight, INT32 yBottom)
{
	HGDI_RECT hRect = (HGDI_RECT)calloc(1, sizeof(GDI_RECT));

	if (!hRect)
		return NULL;

	hRect->objectType = GDIOBJECT_RECT;
	hRect->left = xLeft;
	hRect->top = yTop;
	hRect->right = xRight;
	hRect->bottom = yBottom;
	return hRect;
}