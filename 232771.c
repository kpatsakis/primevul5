INLINE BOOL gdi_SetRect(HGDI_RECT rc, INT32 xLeft, INT32 yTop, INT32 xRight, INT32 yBottom)
{
	rc->left = xLeft;
	rc->top = yTop;
	rc->right = xRight;
	rc->bottom = yBottom;
	return TRUE;
}