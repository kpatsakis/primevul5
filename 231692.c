INT32 clear_decompress(CLEAR_CONTEXT* clear, const BYTE* pSrcData, UINT32 SrcSize, UINT32 nWidth,
                       UINT32 nHeight, BYTE* pDstData, UINT32 DstFormat, UINT32 nDstStep,
                       UINT32 nXDst, UINT32 nYDst, UINT32 nDstWidth, UINT32 nDstHeight,
                       const gdiPalette* palette)
{
	INT32 rc = -1;
	BYTE seqNumber;
	BYTE glyphFlags;
	UINT32 residualByteCount;
	UINT32 bandsByteCount;
	UINT32 subcodecByteCount;
	wStream* s;
	BYTE* glyphData = NULL;

	if (!pDstData)
		return -1002;

	if ((nDstWidth == 0) || (nDstHeight == 0))
		return -1022;

	if ((nWidth > 0xFFFF) || (nHeight > 0xFFFF))
		return -1004;

	s = Stream_New((BYTE*)pSrcData, SrcSize);

	if (!s)
		return -2005;

	Stream_SetLength(s, SrcSize);

	if (Stream_GetRemainingLength(s) < 2)
	{
		WLog_ERR(TAG, "stream short %" PRIuz " [2 expected]", Stream_GetRemainingLength(s));
		goto fail;
	}

	if (!updateContextFormat(clear, DstFormat))
		goto fail;

	Stream_Read_UINT8(s, glyphFlags);
	Stream_Read_UINT8(s, seqNumber);

	if (!clear->seqNumber && seqNumber)
		clear->seqNumber = seqNumber;

	if (seqNumber != clear->seqNumber)
	{
		WLog_ERR(TAG, "Sequence number unexpected %" PRIu8 " - %" PRIu32 "", seqNumber,
		         clear->seqNumber);
		WLog_ERR(TAG, "seqNumber %" PRIu8 " != clear->seqNumber %" PRIu32 "", seqNumber,
		         clear->seqNumber);
		goto fail;
	}

	clear->seqNumber = (seqNumber + 1) % 256;

	if (glyphFlags & CLEARCODEC_FLAG_CACHE_RESET)
	{
		clear->VBarStorageCursor = 0;
		clear->ShortVBarStorageCursor = 0;
	}

	if (!clear_decompress_glyph_data(clear, s, glyphFlags, nWidth, nHeight, pDstData, DstFormat,
	                                 nDstStep, nXDst, nYDst, nDstWidth, nDstHeight, palette,
	                                 &glyphData))
	{
		WLog_ERR(TAG, "clear_decompress_glyph_data failed!");
		goto fail;
	}

	/* Read composition payload header parameters */
	if (Stream_GetRemainingLength(s) < 12)
	{
		const UINT32 mask = (CLEARCODEC_FLAG_GLYPH_HIT | CLEARCODEC_FLAG_GLYPH_INDEX);

		if ((glyphFlags & mask) == mask)
			goto finish;

		WLog_ERR(TAG, "stream short %" PRIuz " [12 expected]", Stream_GetRemainingLength(s));
		goto fail;
	}

	Stream_Read_UINT32(s, residualByteCount);
	Stream_Read_UINT32(s, bandsByteCount);
	Stream_Read_UINT32(s, subcodecByteCount);

	if (residualByteCount > 0)
	{
		if (!clear_decompress_residual_data(clear, s, residualByteCount, nWidth, nHeight, pDstData,
		                                    DstFormat, nDstStep, nXDst, nYDst, nDstWidth,
		                                    nDstHeight, palette))
		{
			WLog_ERR(TAG, "clear_decompress_residual_data failed!");
			goto fail;
		}
	}

	if (bandsByteCount > 0)
	{
		if (!clear_decompress_bands_data(clear, s, bandsByteCount, nWidth, nHeight, pDstData,
		                                 DstFormat, nDstStep, nXDst, nYDst))
		{
			WLog_ERR(TAG, "clear_decompress_bands_data failed!");
			goto fail;
		}
	}

	if (subcodecByteCount > 0)
	{
		if (!clear_decompress_subcodecs_data(clear, s, subcodecByteCount, nWidth, nHeight, pDstData,
		                                     DstFormat, nDstStep, nXDst, nYDst, nDstWidth,
		                                     nDstHeight, palette))
		{
			WLog_ERR(TAG, "clear_decompress_subcodecs_data failed!");
			goto fail;
		}
	}

	if (glyphData)
	{
		if (!freerdp_image_copy(glyphData, clear->format, 0, 0, 0, nWidth, nHeight, pDstData,
		                        DstFormat, nDstStep, nXDst, nYDst, palette, FREERDP_FLIP_NONE))
			goto fail;
	}

finish:
	rc = 0;
fail:
	Stream_Free(s, FALSE);
	return rc;
}