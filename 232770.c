INLINE void gdi_RectToCRgn(const HGDI_RECT rect, INT32* x, INT32* y, INT32* w, INT32* h)
{
	INT64 tmp;
	*x = rect->left;
	*y = rect->top;
	tmp = rect->right - rect->left + 1;
	if ((tmp < 0) || (tmp > INT32_MAX))
	{
		char buffer[256];
		WLog_ERR(TAG, "[%s] rectangle invalid %s", __FUNCTION__,
		         gdi_rect_str(buffer, sizeof(buffer), rect));
		*w = 0;
	}
	else
		*w = tmp;
	tmp = rect->bottom - rect->top + 1;
	if ((tmp < 0) || (tmp > INT32_MAX))
	{
		char buffer[256];
		WLog_ERR(TAG, "[%s] rectangle invalid %s", __FUNCTION__,
		         gdi_rect_str(buffer, sizeof(buffer), rect));
		*h = 0;
	}
	else
		*h = tmp;
}