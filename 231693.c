static BOOL clear_decompress_subcodecs_data(CLEAR_CONTEXT* clear, wStream* s,
                                            UINT32 subcodecByteCount, UINT32 nWidth, UINT32 nHeight,
                                            BYTE* pDstData, UINT32 DstFormat, UINT32 nDstStep,
                                            UINT32 nXDst, UINT32 nYDst, UINT32 nDstWidth,
                                            UINT32 nDstHeight, const gdiPalette* palette)
{
	UINT16 xStart;
	UINT16 yStart;
	UINT16 width;
	UINT16 height;
	UINT32 bitmapDataByteCount;
	BYTE subcodecId;
	UINT32 suboffset;

	if (Stream_GetRemainingLength(s) < subcodecByteCount)
	{
		WLog_ERR(TAG, "stream short %" PRIuz " [%" PRIu32 " expected]",
		         Stream_GetRemainingLength(s), subcodecByteCount);
		return FALSE;
	}

	suboffset = 0;

	while (suboffset < subcodecByteCount)
	{
		UINT32 nXDstRel;
		UINT32 nYDstRel;

		if (Stream_GetRemainingLength(s) < 13)
		{
			WLog_ERR(TAG, "stream short %" PRIuz " [13 expected]", Stream_GetRemainingLength(s));
			return FALSE;
		}

		Stream_Read_UINT16(s, xStart);
		Stream_Read_UINT16(s, yStart);
		Stream_Read_UINT16(s, width);
		Stream_Read_UINT16(s, height);
		Stream_Read_UINT32(s, bitmapDataByteCount);
		Stream_Read_UINT8(s, subcodecId);
		suboffset += 13;

		if (Stream_GetRemainingLength(s) < bitmapDataByteCount)
		{
			WLog_ERR(TAG, "stream short %" PRIuz " [%" PRIu32 " expected]",
			         Stream_GetRemainingLength(s), bitmapDataByteCount);
			return FALSE;
		}

		nXDstRel = nXDst + xStart;
		nYDstRel = nYDst + yStart;

		if (width > nWidth)
		{
			WLog_ERR(TAG, "width %" PRIu16 " > nWidth %" PRIu32 "", width, nWidth);
			return FALSE;
		}

		if (height > nHeight)
		{
			WLog_ERR(TAG, "height %" PRIu16 " > nHeight %" PRIu32 "", height, nHeight);
			return FALSE;
		}

		if (!clear_resize_buffer(clear, width, height))
			return FALSE;

		switch (subcodecId)
		{
			case 0: /* Uncompressed */
			{
				UINT32 nSrcStep = width * GetBytesPerPixel(PIXEL_FORMAT_BGR24);
				UINT32 nSrcSize = nSrcStep * height;

				if (bitmapDataByteCount != nSrcSize)
				{
					WLog_ERR(TAG, "bitmapDataByteCount %" PRIu32 " != nSrcSize %" PRIu32 "",
					         bitmapDataByteCount, nSrcSize);
					return FALSE;
				}

				if (!convert_color(pDstData, nDstStep, DstFormat, nXDstRel, nYDstRel, width, height,
				                   Stream_Pointer(s), nSrcStep, PIXEL_FORMAT_BGR24, nDstWidth,
				                   nDstHeight, palette))
					return FALSE;

				Stream_Seek(s, bitmapDataByteCount);
			}
			break;

			case 1: /* NSCodec */
				if (!clear_decompress_nscodec(clear->nsc, width, height, s, bitmapDataByteCount,
				                              pDstData, DstFormat, nDstStep, nXDstRel, nYDstRel))
					return FALSE;

				break;

			case 2: /* CLEARCODEC_SUBCODEC_RLEX */
				if (!clear_decompress_subcode_rlex(s, bitmapDataByteCount, width, height, pDstData,
				                                   DstFormat, nDstStep, nXDstRel, nYDstRel,
				                                   nDstWidth, nDstHeight))
					return FALSE;

				break;

			default:
				WLog_ERR(TAG, "Unknown subcodec ID %" PRIu8 "", subcodecId);
				return FALSE;
		}

		suboffset += bitmapDataByteCount;
	}

	return TRUE;
}