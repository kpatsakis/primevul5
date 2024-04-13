BOOL interleaved_decompress(BITMAP_INTERLEAVED_CONTEXT* interleaved, const BYTE* pSrcData,
                            UINT32 SrcSize, UINT32 nSrcWidth, UINT32 nSrcHeight, UINT32 bpp,
                            BYTE* pDstData, UINT32 DstFormat, UINT32 nDstStep, UINT32 nXDst,
                            UINT32 nYDst, UINT32 nDstWidth, UINT32 nDstHeight,
                            const gdiPalette* palette)
{
	UINT32 scanline;
	UINT32 SrcFormat;
	UINT32 BufferSize;

	if (!interleaved || !pSrcData || !pDstData)
		return FALSE;

	switch (bpp)
	{
		case 24:
			scanline = nSrcWidth * 3;
			SrcFormat = PIXEL_FORMAT_BGR24;
			break;

		case 16:
			scanline = nSrcWidth * 2;
			SrcFormat = PIXEL_FORMAT_RGB16;
			break;

		case 15:
			scanline = nSrcWidth * 2;
			SrcFormat = PIXEL_FORMAT_RGB15;
			break;

		case 8:
			scanline = nSrcWidth;
			SrcFormat = PIXEL_FORMAT_RGB8;
			break;

		default:
			WLog_ERR(TAG, "Invalid color depth %" PRIu32 "", bpp);
			return FALSE;
	}

	BufferSize = scanline * nSrcHeight;

	if (BufferSize > interleaved->TempSize)
	{
		interleaved->TempBuffer = _aligned_realloc(interleaved->TempBuffer, BufferSize, 16);
		interleaved->TempSize = BufferSize;
	}

	if (!interleaved->TempBuffer)
		return FALSE;

	switch (bpp)
	{
		case 24:
			if (!RleDecompress24to24(pSrcData, SrcSize, interleaved->TempBuffer, scanline,
			                         nSrcWidth, nSrcHeight))
				return FALSE;

			break;

		case 16:
		case 15:
			if (!RleDecompress16to16(pSrcData, SrcSize, interleaved->TempBuffer, scanline,
			                         nSrcWidth, nSrcHeight))
				return FALSE;

			break;

		case 8:
			if (!RleDecompress8to8(pSrcData, SrcSize, interleaved->TempBuffer, scanline, nSrcWidth,
			                       nSrcHeight))
				return FALSE;

			break;

		default:
			return FALSE;
	}

	return freerdp_image_copy(pDstData, DstFormat, nDstStep, nXDst, nYDst, nDstWidth, nDstHeight,
	                          interleaved->TempBuffer, SrcFormat, scanline, 0, 0, palette,
	                          FREERDP_FLIP_VERTICAL);
}