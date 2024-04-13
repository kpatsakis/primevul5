static INLINE void write_pixel_24(BYTE* _buf, UINT32 _pix)
{
	(_buf)[0] = (BYTE)(_pix);
	(_buf)[1] = (BYTE)((_pix) >> 8);
	(_buf)[2] = (BYTE)((_pix) >> 16);
}