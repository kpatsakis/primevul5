static BOOL clear_decompress_residual_data(CLEAR_CONTEXT* clear, wStream* s,
                                           UINT32 residualByteCount, UINT32 nWidth, UINT32 nHeight,
                                           BYTE* pDstData, UINT32 DstFormat, UINT32 nDstStep,
                                           UINT32 nXDst, UINT32 nYDst, UINT32 nDstWidth,
                                           UINT32 nDstHeight, const gdiPalette* palette)
{
	UINT32 i;
	UINT32 nSrcStep;
	UINT32 suboffset;
	BYTE* dstBuffer;
	UINT32 pixelIndex;
	UINT32 pixelCount;

	if (Stream_GetRemainingLength(s) < residualByteCount)
	{
		WLog_ERR(TAG, "stream short %" PRIuz " [%" PRIu32 " expected]",
		         Stream_GetRemainingLength(s), residualByteCount);
		return FALSE;
	}

	suboffset = 0;
	pixelIndex = 0;
	pixelCount = nWidth * nHeight;

	if (!clear_resize_buffer(clear, nWidth, nHeight))
		return FALSE;

	dstBuffer = clear->TempBuffer;

	while (suboffset < residualByteCount)
	{
		BYTE r, g, b;
		UINT32 runLengthFactor;
		UINT32 color;

		if (Stream_GetRemainingLength(s) < 4)
		{
			WLog_ERR(TAG, "stream short %" PRIuz " [4 expected]", Stream_GetRemainingLength(s));
			return FALSE;
		}

		Stream_Read_UINT8(s, b);
		Stream_Read_UINT8(s, g);
		Stream_Read_UINT8(s, r);
		Stream_Read_UINT8(s, runLengthFactor);
		suboffset += 4;
		color = FreeRDPGetColor(clear->format, r, g, b, 0xFF);

		if (runLengthFactor >= 0xFF)
		{
			if (Stream_GetRemainingLength(s) < 2)
			{
				WLog_ERR(TAG, "stream short %" PRIuz " [2 expected]", Stream_GetRemainingLength(s));
				return FALSE;
			}

			Stream_Read_UINT16(s, runLengthFactor);
			suboffset += 2;

			if (runLengthFactor >= 0xFFFF)
			{
				if (Stream_GetRemainingLength(s) < 4)
				{
					WLog_ERR(TAG, "stream short %" PRIuz " [4 expected]",
					         Stream_GetRemainingLength(s));
					return FALSE;
				}

				Stream_Read_UINT32(s, runLengthFactor);
				suboffset += 4;
			}
		}

		if ((pixelIndex + runLengthFactor) > pixelCount)
		{
			WLog_ERR(TAG,
			         "pixelIndex %" PRIu32 " + runLengthFactor %" PRIu32 " > pixelCount %" PRIu32
			         "",
			         pixelIndex, runLengthFactor, pixelCount);
			return FALSE;
		}

		for (i = 0; i < runLengthFactor; i++)
		{
			WriteColor(dstBuffer, clear->format, color);
			dstBuffer += GetBytesPerPixel(clear->format);
		}

		pixelIndex += runLengthFactor;
	}

	nSrcStep = nWidth * GetBytesPerPixel(clear->format);

	if (pixelIndex != pixelCount)
	{
		WLog_ERR(TAG, "pixelIndex %" PRIu32 " != pixelCount %" PRIu32 "", pixelIndex, pixelCount);
		return FALSE;
	}

	return convert_color(pDstData, nDstStep, DstFormat, nXDst, nYDst, nWidth, nHeight,
	                     clear->TempBuffer, nSrcStep, clear->format, nDstWidth, nDstHeight,
	                     palette);
}