BOOL interleaved_compress(BITMAP_INTERLEAVED_CONTEXT* interleaved, BYTE* pDstData, UINT32* pDstSize,
                          UINT32 nWidth, UINT32 nHeight, const BYTE* pSrcData, UINT32 SrcFormat,
                          UINT32 nSrcStep, UINT32 nXSrc, UINT32 nYSrc, const gdiPalette* palette,
                          UINT32 bpp)
{
	BOOL status;
	wStream* s;
	UINT32 DstFormat = 0;
	const size_t maxSize = 64 * 64 * 4;

	if (!interleaved || !pDstData || !pSrcData)
		return FALSE;

	if ((nWidth == 0) || (nHeight == 0))
		return FALSE;

	if (nWidth % 4)
	{
		WLog_ERR(TAG, "interleaved_compress: width is not a multiple of 4");
		return FALSE;
	}

	if ((nWidth > 64) || (nHeight > 64))
	{
		WLog_ERR(TAG,
		         "interleaved_compress: width (%" PRIu32 ") or height (%" PRIu32
		         ") is greater than 64",
		         nWidth, nHeight);
		return FALSE;
	}

	switch (bpp)
	{
		case 24:
			DstFormat = PIXEL_FORMAT_BGRX32;
			break;

		case 16:
			DstFormat = PIXEL_FORMAT_RGB16;
			break;

		case 15:
			DstFormat = PIXEL_FORMAT_RGB15;
			break;

		default:
			return FALSE;
	}

	if (!freerdp_image_copy(interleaved->TempBuffer, DstFormat, 0, 0, 0, nWidth, nHeight, pSrcData,
	                        SrcFormat, nSrcStep, nXSrc, nYSrc, palette, FREERDP_FLIP_NONE))
		return FALSE;

	s = Stream_New(pDstData, *pDstSize);

	if (!s)
		return FALSE;

	Stream_SetPosition(interleaved->bts, 0);

	if (freerdp_bitmap_compress(interleaved->TempBuffer, nWidth, nHeight, s, bpp, maxSize,
	                            nHeight - 1, interleaved->bts, 0) < 0)
		status = FALSE;
	else
		status = TRUE;

	Stream_SealLength(s);
	*pDstSize = (UINT32)Stream_Length(s);
	Stream_Free(s, FALSE);
	return status;
}