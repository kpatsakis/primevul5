INLINE void gdi_CRectToCRgn(INT32 left, INT32 top, INT32 right, INT32 bottom, INT32* x, INT32* y,
                            INT32* w, INT32* h)
{
	*x = left;
	*y = top;
	*w = right - left + 1;
	*h = bottom - top + 1;
}