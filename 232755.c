INLINE void gdi_CRgnToCRect(INT32 x, INT32 y, INT32 w, INT32 h, INT32* left, INT32* top,
                            INT32* right, INT32* bottom)
{
	*left = x;
	*top = y;
	*right = 0;

	if (w > 0)
		*right = x + w - 1;
	else
		WLog_ERR(TAG, "Invalid width");

	*bottom = 0;

	if (h > 0)
		*bottom = y + h - 1;
	else
		WLog_ERR(TAG, "Invalid height");
}