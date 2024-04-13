INLINE void gdi_CRgnToRect(INT64 x, INT64 y, INT32 w, INT32 h, HGDI_RECT rect)
{
	BOOL invalid = FALSE;
	const INT64 r = x + w - 1;
	const INT64 b = y + h - 1;
	rect->left = (x > 0) ? x : 0;
	rect->top = (y > 0) ? y : 0;
	rect->right = rect->left;
	rect->bottom = rect->top;

	if (r > 0)
		rect->right = r;
	else
		invalid = TRUE;

	if (b > 0)
		rect->bottom = b;
	else
		invalid = TRUE;

	if (invalid)
	{
		WLog_DBG(TAG, "Invisible rectangle %" PRId64 "x%" PRId64 "-%" PRId64 "x%" PRId64, x, y, r,
		         b);
	}
}