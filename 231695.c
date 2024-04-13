static BOOL clear_decompress_bands_data(CLEAR_CONTEXT* clear, wStream* s, UINT32 bandsByteCount,
                                        UINT32 nWidth, UINT32 nHeight, BYTE* pDstData,
                                        UINT32 DstFormat, UINT32 nDstStep, UINT32 nXDst,
                                        UINT32 nYDst)
{
	UINT32 i, y;
	UINT32 count;
	UINT32 suboffset;
	UINT32 nXDstRel;
	UINT32 nYDstRel;

	if (Stream_GetRemainingLength(s) < bandsByteCount)
	{
		WLog_ERR(TAG, "stream short %" PRIuz " [11 expected]", Stream_GetRemainingLength(s));
		return FALSE;
	}

	suboffset = 0;

	while (suboffset < bandsByteCount)
	{
		BYTE r, g, b;
		UINT16 xStart;
		UINT16 xEnd;
		UINT16 yStart;
		UINT16 yEnd;
		UINT32 colorBkg;
		UINT16 vBarHeader;
		UINT16 vBarYOn;
		UINT16 vBarYOff;
		UINT32 vBarCount;
		UINT32 vBarPixelCount;
		UINT32 vBarShortPixelCount;

		if (Stream_GetRemainingLength(s) < 11)
		{
			WLog_ERR(TAG, "stream short %" PRIuz " [11 expected]", Stream_GetRemainingLength(s));
			return FALSE;
		}

		Stream_Read_UINT16(s, xStart);
		Stream_Read_UINT16(s, xEnd);
		Stream_Read_UINT16(s, yStart);
		Stream_Read_UINT16(s, yEnd);
		Stream_Read_UINT8(s, b);
		Stream_Read_UINT8(s, g);
		Stream_Read_UINT8(s, r);
		suboffset += 11;
		colorBkg = FreeRDPGetColor(clear->format, r, g, b, 0xFF);

		if (xEnd < xStart)
		{
			WLog_ERR(TAG, "xEnd %" PRIu16 " < xStart %" PRIu16 "", xEnd, xStart);
			return FALSE;
		}

		if (yEnd < yStart)
		{
			WLog_ERR(TAG, "yEnd %" PRIu16 " < yStart %" PRIu16 "", yEnd, yStart);
			return FALSE;
		}

		vBarCount = (xEnd - xStart) + 1;

		for (i = 0; i < vBarCount; i++)
		{
			UINT32 vBarHeight;
			CLEAR_VBAR_ENTRY* vBarEntry = NULL;
			CLEAR_VBAR_ENTRY* vBarShortEntry;
			BOOL vBarUpdate = FALSE;
			const BYTE* pSrcPixel;

			if (Stream_GetRemainingLength(s) < 2)
			{
				WLog_ERR(TAG, "stream short %" PRIuz " [2 expected]", Stream_GetRemainingLength(s));
				return FALSE;
			}

			Stream_Read_UINT16(s, vBarHeader);
			suboffset += 2;
			vBarHeight = (yEnd - yStart + 1);

			if (vBarHeight > 52)
			{
				WLog_ERR(TAG, "vBarHeight (%" PRIu32 ") > 52", vBarHeight);
				return FALSE;
			}

			if ((vBarHeader & 0xC000) == 0x4000) /* SHORT_VBAR_CACHE_HIT */
			{
				const UINT16 vBarIndex = (vBarHeader & 0x3FFF);
				vBarShortEntry = &(clear->ShortVBarStorage[vBarIndex]);

				if (!vBarShortEntry)
				{
					WLog_ERR(TAG, "missing vBarShortEntry %" PRIu16 "", vBarIndex);
					return FALSE;
				}

				if (Stream_GetRemainingLength(s) < 1)
				{
					WLog_ERR(TAG, "stream short %" PRIuz " [1 expected]",
					         Stream_GetRemainingLength(s));
					return FALSE;
				}

				Stream_Read_UINT8(s, vBarYOn);
				suboffset += 1;
				vBarShortPixelCount = vBarShortEntry->count;
				vBarUpdate = TRUE;
			}
			else if ((vBarHeader & 0xC000) == 0x0000) /* SHORT_VBAR_CACHE_MISS */
			{
				vBarYOn = (vBarHeader & 0xFF);
				vBarYOff = ((vBarHeader >> 8) & 0x3F);

				if (vBarYOff < vBarYOn)
				{
					WLog_ERR(TAG, "vBarYOff %" PRIu16 " < vBarYOn %" PRIu16 "", vBarYOff, vBarYOn);
					return FALSE;
				}

				vBarShortPixelCount = (vBarYOff - vBarYOn);

				if (vBarShortPixelCount > 52)
				{
					WLog_ERR(TAG, "vBarShortPixelCount %" PRIu32 " > 52", vBarShortPixelCount);
					return FALSE;
				}

				if (Stream_GetRemainingLength(s) < (vBarShortPixelCount * 3))
				{
					WLog_ERR(TAG, "stream short %" PRIuz " [%" PRIu32 " expected]",
					         Stream_GetRemainingLength(s), (vBarShortPixelCount * 3));
					return FALSE;
				}

				if (clear->ShortVBarStorageCursor >= CLEARCODEC_VBAR_SHORT_SIZE)
				{
					WLog_ERR(TAG,
					         "clear->ShortVBarStorageCursor %" PRIu32
					         " >= CLEARCODEC_VBAR_SHORT_SIZE (%" PRIu32 ")",
					         clear->ShortVBarStorageCursor, CLEARCODEC_VBAR_SHORT_SIZE);
					return FALSE;
				}

				vBarShortEntry = &(clear->ShortVBarStorage[clear->ShortVBarStorageCursor]);
				vBarShortEntry->count = vBarShortPixelCount;

				if (!resize_vbar_entry(clear, vBarShortEntry))
					return FALSE;

				for (y = 0; y < vBarShortPixelCount; y++)
				{
					BYTE r, g, b;
					BYTE* dstBuffer = &vBarShortEntry->pixels[y * GetBytesPerPixel(clear->format)];
					UINT32 color;
					Stream_Read_UINT8(s, b);
					Stream_Read_UINT8(s, g);
					Stream_Read_UINT8(s, r);
					color = FreeRDPGetColor(clear->format, r, g, b, 0xFF);

					if (!WriteColor(dstBuffer, clear->format, color))
						return FALSE;
				}

				suboffset += (vBarShortPixelCount * 3);
				clear->ShortVBarStorageCursor =
				    (clear->ShortVBarStorageCursor + 1) % CLEARCODEC_VBAR_SHORT_SIZE;
				vBarUpdate = TRUE;
			}
			else if ((vBarHeader & 0x8000) == 0x8000) /* VBAR_CACHE_HIT */
			{
				const UINT16 vBarIndex = (vBarHeader & 0x7FFF);
				vBarEntry = &(clear->VBarStorage[vBarIndex]);

				/* If the cache was reset we need to fill in some dummy data. */
				if (vBarEntry->size == 0)
				{
					WLog_WARN(TAG, "Empty cache index %" PRIu16 ", filling dummy data", vBarIndex);
					vBarEntry->count = vBarHeight;

					if (!resize_vbar_entry(clear, vBarEntry))
						return FALSE;
				}
			}
			else
			{
				WLog_ERR(TAG, "invalid vBarHeader 0x%04" PRIX16 "", vBarHeader);
				return FALSE; /* invalid vBarHeader */
			}

			if (vBarUpdate)
			{
				UINT32 x;
				BYTE* pSrcPixel;
				BYTE* dstBuffer;

				if (clear->VBarStorageCursor >= CLEARCODEC_VBAR_SIZE)
				{
					WLog_ERR(TAG,
					         "clear->VBarStorageCursor %" PRIu32 " >= CLEARCODEC_VBAR_SIZE %" PRIu32
					         "",
					         clear->VBarStorageCursor, CLEARCODEC_VBAR_SIZE);
					return FALSE;
				}

				vBarEntry = &(clear->VBarStorage[clear->VBarStorageCursor]);
				vBarPixelCount = vBarHeight;
				vBarEntry->count = vBarPixelCount;

				if (!resize_vbar_entry(clear, vBarEntry))
					return FALSE;

				dstBuffer = vBarEntry->pixels;
				/* if (y < vBarYOn), use colorBkg */
				y = 0;
				count = vBarYOn;

				if ((y + count) > vBarPixelCount)
					count = (vBarPixelCount > y) ? (vBarPixelCount - y) : 0;

				while (count--)
				{
					WriteColor(dstBuffer, clear->format, colorBkg);
					dstBuffer += GetBytesPerPixel(clear->format);
				}

				/*
				 * if ((y >= vBarYOn) && (y < (vBarYOn + vBarShortPixelCount))),
				 * use vBarShortPixels at index (y - shortVBarYOn)
				 */
				y = vBarYOn;
				count = vBarShortPixelCount;

				if ((y + count) > vBarPixelCount)
					count = (vBarPixelCount > y) ? (vBarPixelCount - y) : 0;

				pSrcPixel =
				    &vBarShortEntry->pixels[(y - vBarYOn) * GetBytesPerPixel(clear->format)];

				for (x = 0; x < count; x++)
				{
					UINT32 color;
					color =
					    ReadColor(&pSrcPixel[x * GetBytesPerPixel(clear->format)], clear->format);

					if (!WriteColor(dstBuffer, clear->format, color))
						return FALSE;

					dstBuffer += GetBytesPerPixel(clear->format);
				}

				/* if (y >= (vBarYOn + vBarShortPixelCount)), use colorBkg */
				y = vBarYOn + vBarShortPixelCount;
				count = (vBarPixelCount > y) ? (vBarPixelCount - y) : 0;

				while (count--)
				{
					if (!WriteColor(dstBuffer, clear->format, colorBkg))
						return FALSE;

					dstBuffer += GetBytesPerPixel(clear->format);
				}

				vBarEntry->count = vBarPixelCount;
				clear->VBarStorageCursor = (clear->VBarStorageCursor + 1) % CLEARCODEC_VBAR_SIZE;
			}

			if (vBarEntry->count != vBarHeight)
			{
				WLog_ERR(TAG, "vBarEntry->count %" PRIu32 " != vBarHeight %" PRIu32 "",
				         vBarEntry->count, vBarHeight);
				vBarEntry->count = vBarHeight;

				if (!resize_vbar_entry(clear, vBarEntry))
					return FALSE;
			}

			nXDstRel = nXDst + xStart;
			nYDstRel = nYDst + yStart;
			pSrcPixel = vBarEntry->pixels;

			if (i < nWidth)
			{
				count = vBarEntry->count;

				if (count > nHeight)
					count = nHeight;

				for (y = 0; y < count; y++)
				{
					BYTE* pDstPixel8 = &pDstData[((nYDstRel + y) * nDstStep) +
					                             ((nXDstRel + i) * GetBytesPerPixel(DstFormat))];
					UINT32 color = ReadColor(pSrcPixel, clear->format);
					color = FreeRDPConvertColor(color, clear->format, DstFormat, NULL);

					if (!WriteColor(pDstPixel8, DstFormat, color))
						return FALSE;

					pSrcPixel += GetBytesPerPixel(clear->format);
				}
			}
		}
	}

	return TRUE;
}