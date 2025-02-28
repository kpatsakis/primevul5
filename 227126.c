static INLINE BOOL RLEDECOMPRESS(const BYTE* pbSrcBuffer, UINT32 cbSrcBuffer, BYTE* pbDestBuffer,
                                 UINT32 rowDelta, UINT32 width, UINT32 height)
{
	const BYTE* pbSrc = pbSrcBuffer;
	const BYTE* pbEnd;
	const BYTE* pbDestEnd;
	BYTE* pbDest = pbDestBuffer;
	PIXEL temp;
	PIXEL fgPel = WHITE_PIXEL;
	BOOL fInsertFgPel = FALSE;
	BOOL fFirstLine = TRUE;
	BYTE bitmask;
	PIXEL pixelA, pixelB;
	UINT32 runLength;
	UINT32 code;
	UINT32 advance;
	RLEEXTRA

	if ((rowDelta == 0) || (rowDelta < width))
		return FALSE;

	if (!pbSrcBuffer || !pbDestBuffer)
		return FALSE;

	pbEnd = pbSrcBuffer + cbSrcBuffer;
	pbDestEnd = pbDestBuffer + rowDelta * height;

	while (pbSrc < pbEnd)
	{
		/* Watch out for the end of the first scanline. */
		if (fFirstLine)
		{
			if ((UINT32)(pbDest - pbDestBuffer) >= rowDelta)
			{
				fFirstLine = FALSE;
				fInsertFgPel = FALSE;
			}
		}

		/*
		   Extract the compression order code ID from the compression
		   order header.
		*/
		code = ExtractCodeId(*pbSrc);

		/* Handle Background Run Orders. */
		if (code == REGULAR_BG_RUN || code == MEGA_MEGA_BG_RUN)
		{
			runLength = ExtractRunLength(code, pbSrc, &advance);
			pbSrc = pbSrc + advance;

			if (fFirstLine)
			{
				if (fInsertFgPel)
				{
					if (!ENSURE_CAPACITY(pbDest, pbDestEnd, 1))
						return FALSE;

					DESTWRITEPIXEL(pbDest, fgPel);
					DESTNEXTPIXEL(pbDest);
					runLength = runLength - 1;
				}

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, runLength))
					return FALSE;

				UNROLL(runLength, {
					DESTWRITEPIXEL(pbDest, BLACK_PIXEL);
					DESTNEXTPIXEL(pbDest);
				});
			}
			else
			{
				if (fInsertFgPel)
				{
					DESTREADPIXEL(temp, pbDest - rowDelta);

					if (!ENSURE_CAPACITY(pbDest, pbDestEnd, 1))
						return FALSE;

					DESTWRITEPIXEL(pbDest, temp ^ fgPel);
					DESTNEXTPIXEL(pbDest);
					runLength--;
				}

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, runLength))
					return FALSE;

				UNROLL(runLength, {
					DESTREADPIXEL(temp, pbDest - rowDelta);
					DESTWRITEPIXEL(pbDest, temp);
					DESTNEXTPIXEL(pbDest);
				});
			}

			/* A follow-on background run order will need a foreground pel inserted. */
			fInsertFgPel = TRUE;
			continue;
		}

		/* For any of the other run-types a follow-on background run
		    order does not need a foreground pel inserted. */
		fInsertFgPel = FALSE;

		switch (code)
		{
			/* Handle Foreground Run Orders. */
			case REGULAR_FG_RUN:
			case MEGA_MEGA_FG_RUN:
			case LITE_SET_FG_FG_RUN:
			case MEGA_MEGA_SET_FG_RUN:
				runLength = ExtractRunLength(code, pbSrc, &advance);
				pbSrc = pbSrc + advance;

				if (code == LITE_SET_FG_FG_RUN || code == MEGA_MEGA_SET_FG_RUN)
				{
					if (pbSrc >= pbEnd)
						return FALSE;
					SRCREADPIXEL(fgPel, pbSrc);
					SRCNEXTPIXEL(pbSrc);
				}

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, runLength))
					return FALSE;

				if (fFirstLine)
				{
					UNROLL(runLength, {
						DESTWRITEPIXEL(pbDest, fgPel);
						DESTNEXTPIXEL(pbDest);
					});
				}
				else
				{
					UNROLL(runLength, {
						DESTREADPIXEL(temp, pbDest - rowDelta);
						DESTWRITEPIXEL(pbDest, temp ^ fgPel);
						DESTNEXTPIXEL(pbDest);
					});
				}

				break;

			/* Handle Dithered Run Orders. */
			case LITE_DITHERED_RUN:
			case MEGA_MEGA_DITHERED_RUN:
				runLength = ExtractRunLength(code, pbSrc, &advance);
				pbSrc = pbSrc + advance;
				if (pbSrc >= pbEnd)
					return FALSE;
				SRCREADPIXEL(pixelA, pbSrc);
				SRCNEXTPIXEL(pbSrc);
				if (pbSrc >= pbEnd)
					return FALSE;
				SRCREADPIXEL(pixelB, pbSrc);
				SRCNEXTPIXEL(pbSrc);

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, runLength * 2))
					return FALSE;

				UNROLL(runLength, {
					DESTWRITEPIXEL(pbDest, pixelA);
					DESTNEXTPIXEL(pbDest);
					DESTWRITEPIXEL(pbDest, pixelB);
					DESTNEXTPIXEL(pbDest);
				});
				break;

			/* Handle Color Run Orders. */
			case REGULAR_COLOR_RUN:
			case MEGA_MEGA_COLOR_RUN:
				runLength = ExtractRunLength(code, pbSrc, &advance);
				pbSrc = pbSrc + advance;
				if (pbSrc >= pbEnd)
					return FALSE;
				SRCREADPIXEL(pixelA, pbSrc);
				SRCNEXTPIXEL(pbSrc);

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, runLength))
					return FALSE;

				UNROLL(runLength, {
					DESTWRITEPIXEL(pbDest, pixelA);
					DESTNEXTPIXEL(pbDest);
				});
				break;

			/* Handle Foreground/Background Image Orders. */
			case REGULAR_FGBG_IMAGE:
			case MEGA_MEGA_FGBG_IMAGE:
			case LITE_SET_FG_FGBG_IMAGE:
			case MEGA_MEGA_SET_FGBG_IMAGE:
				runLength = ExtractRunLength(code, pbSrc, &advance);
				pbSrc = pbSrc + advance;

				if (pbSrc >= pbEnd)
					return FALSE;
				if (code == LITE_SET_FG_FGBG_IMAGE || code == MEGA_MEGA_SET_FGBG_IMAGE)
				{
					SRCREADPIXEL(fgPel, pbSrc);
					SRCNEXTPIXEL(pbSrc);
				}

				if (fFirstLine)
				{
					while (runLength > 8)
					{
						bitmask = *pbSrc;
						pbSrc = pbSrc + 1;
						pbDest = WRITEFIRSTLINEFGBGIMAGE(pbDest, pbDestEnd, bitmask, fgPel, 8);

						if (!pbDest)
							return FALSE;

						runLength = runLength - 8;
					}
				}
				else
				{
					while (runLength > 8)
					{
						bitmask = *pbSrc;
						pbSrc = pbSrc + 1;
						pbDest = WRITEFGBGIMAGE(pbDest, pbDestEnd, rowDelta, bitmask, fgPel, 8);

						if (!pbDest)
							return FALSE;

						runLength = runLength - 8;
					}
				}

				if (runLength > 0)
				{
					bitmask = *pbSrc;
					pbSrc = pbSrc + 1;

					if (fFirstLine)
					{
						pbDest =
						    WRITEFIRSTLINEFGBGIMAGE(pbDest, pbDestEnd, bitmask, fgPel, runLength);
					}
					else
					{
						pbDest =
						    WRITEFGBGIMAGE(pbDest, pbDestEnd, rowDelta, bitmask, fgPel, runLength);
					}

					if (!pbDest)
						return FALSE;
				}

				break;

			/* Handle Color Image Orders. */
			case REGULAR_COLOR_IMAGE:
			case MEGA_MEGA_COLOR_IMAGE:
				runLength = ExtractRunLength(code, pbSrc, &advance);
				pbSrc = pbSrc + advance;
				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, runLength))
					return FALSE;

				UNROLL(runLength, {
					if (pbSrc >= pbEnd)
						return FALSE;
					SRCREADPIXEL(temp, pbSrc);
					SRCNEXTPIXEL(pbSrc);
					DESTWRITEPIXEL(pbDest, temp);
					DESTNEXTPIXEL(pbDest);
				});
				break;

			/* Handle Special Order 1. */
			case SPECIAL_FGBG_1:
				pbSrc = pbSrc + 1;

				if (fFirstLine)
				{
					pbDest =
					    WRITEFIRSTLINEFGBGIMAGE(pbDest, pbDestEnd, g_MaskSpecialFgBg1, fgPel, 8);
				}
				else
				{
					pbDest =
					    WRITEFGBGIMAGE(pbDest, pbDestEnd, rowDelta, g_MaskSpecialFgBg1, fgPel, 8);
				}

				if (!pbDest)
					return FALSE;

				break;

			/* Handle Special Order 2. */
			case SPECIAL_FGBG_2:
				pbSrc = pbSrc + 1;

				if (fFirstLine)
				{
					pbDest =
					    WRITEFIRSTLINEFGBGIMAGE(pbDest, pbDestEnd, g_MaskSpecialFgBg2, fgPel, 8);
				}
				else
				{
					pbDest =
					    WRITEFGBGIMAGE(pbDest, pbDestEnd, rowDelta, g_MaskSpecialFgBg2, fgPel, 8);
				}

				if (!pbDest)
					return FALSE;

				break;

			/* Handle White Order. */
			case SPECIAL_WHITE:
				pbSrc = pbSrc + 1;

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, 1))
					return FALSE;

				DESTWRITEPIXEL(pbDest, WHITE_PIXEL);
				DESTNEXTPIXEL(pbDest);
				break;

			/* Handle Black Order. */
			case SPECIAL_BLACK:
				pbSrc = pbSrc + 1;

				if (!ENSURE_CAPACITY(pbDest, pbDestEnd, 1))
					return FALSE;

				DESTWRITEPIXEL(pbDest, BLACK_PIXEL);
				DESTNEXTPIXEL(pbDest);
				break;

			default:
				return FALSE;
		}
	}

	return TRUE;
}