static BOOL convert_color(BYTE* dst, UINT32 nDstStep, UINT32 DstFormat, UINT32 nXDst, UINT32 nYDst,
                          UINT32 nWidth, UINT32 nHeight, const BYTE* src, UINT32 nSrcStep,
                          UINT32 SrcFormat, UINT32 nDstWidth, UINT32 nDstHeight,
                          const gdiPalette* palette)
{
	if (nWidth + nXDst > nDstWidth)
		nWidth = nDstWidth - nXDst;

	if (nHeight + nYDst > nDstHeight)
		nHeight = nDstHeight - nYDst;

	return freerdp_image_copy(dst, DstFormat, nDstStep, nXDst, nYDst, nWidth, nHeight, src,
	                          SrcFormat, nSrcStep, 0, 0, palette, 0);
}