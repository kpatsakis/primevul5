INLINE BOOL gdi_CopyOverlap(INT32 x, INT32 y, INT32 width, INT32 height, INT32 srcx, INT32 srcy)
{
	GDI_RECT dst;
	GDI_RECT src;
	gdi_CRgnToRect(x, y, width, height, &dst);
	gdi_CRgnToRect(srcx, srcy, width, height, &src);
	return (dst.right >= src.left && dst.left <= src.right && dst.bottom >= src.top &&
	        dst.top <= src.bottom)
	           ? TRUE
	           : FALSE;
}