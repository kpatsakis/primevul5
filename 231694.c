static BOOL clear_decompress_glyph_data(CLEAR_CONTEXT* clear, wStream* s, UINT32 glyphFlags,
                                        UINT32 nWidth, UINT32 nHeight, BYTE* pDstData,
                                        UINT32 DstFormat, UINT32 nDstStep, UINT32 nXDst,
                                        UINT32 nYDst, UINT32 nDstWidth, UINT32 nDstHeight,
                                        const gdiPalette* palette, BYTE** ppGlyphData)
{
	UINT16 glyphIndex = 0;

	if (ppGlyphData)
		*ppGlyphData = NULL;

	if ((glyphFlags & CLEARCODEC_FLAG_GLYPH_HIT) && !(glyphFlags & CLEARCODEC_FLAG_GLYPH_INDEX))
	{
		WLog_ERR(TAG, "Invalid glyph flags %08" PRIX32 "", glyphFlags);
		return FALSE;
	}

	if ((glyphFlags & CLEARCODEC_FLAG_GLYPH_INDEX) == 0)
		return TRUE;

	if ((nWidth * nHeight) > (1024 * 1024))
	{
		WLog_ERR(TAG, "glyph too large: %" PRIu32 "x%" PRIu32 "", nWidth, nHeight);
		return FALSE;
	}

	if (Stream_GetRemainingLength(s) < 2)
	{
		WLog_ERR(TAG, "stream short %" PRIuz " [2 expected]", Stream_GetRemainingLength(s));
		return FALSE;
	}

	Stream_Read_UINT16(s, glyphIndex);

	if (glyphIndex >= 4000)
	{
		WLog_ERR(TAG, "Invalid glyphIndex %" PRIu16 "", glyphIndex);
		return FALSE;
	}

	if (glyphFlags & CLEARCODEC_FLAG_GLYPH_HIT)
	{
		UINT32 nSrcStep;
		CLEAR_GLYPH_ENTRY* glyphEntry = &(clear->GlyphCache[glyphIndex]);
		BYTE* glyphData;

		if (!glyphEntry)
		{
			WLog_ERR(TAG, "clear->GlyphCache[%" PRIu16 "]=NULL", glyphIndex);
			return FALSE;
		}

		glyphData = (BYTE*)glyphEntry->pixels;

		if (!glyphData)
		{
			WLog_ERR(TAG, "clear->GlyphCache[%" PRIu16 "]->pixels=NULL", glyphIndex);
			return FALSE;
		}

		if ((nWidth * nHeight) > glyphEntry->count)
		{
			WLog_ERR(TAG,
			         "(nWidth %" PRIu32 " * nHeight %" PRIu32 ") > glyphEntry->count %" PRIu32 "",
			         nWidth, nHeight, glyphEntry->count);
			return FALSE;
		}

		nSrcStep = nWidth * GetBytesPerPixel(clear->format);
		return convert_color(pDstData, nDstStep, DstFormat, nXDst, nYDst, nWidth, nHeight,
		                     glyphData, nSrcStep, clear->format, nDstWidth, nDstHeight, palette);
	}

	if (glyphFlags & CLEARCODEC_FLAG_GLYPH_INDEX)
	{
		const UINT32 bpp = GetBytesPerPixel(clear->format);
		CLEAR_GLYPH_ENTRY* glyphEntry = &(clear->GlyphCache[glyphIndex]);
		glyphEntry->count = nWidth * nHeight;

		if (glyphEntry->count > glyphEntry->size)
		{
			BYTE* tmp;
			tmp = realloc(glyphEntry->pixels, glyphEntry->count * bpp);

			if (!tmp)
			{
				WLog_ERR(TAG, "glyphEntry->pixels realloc %" PRIu32 " failed!",
				         glyphEntry->count * bpp);
				return FALSE;
			}

			glyphEntry->size = glyphEntry->count;
			glyphEntry->pixels = (UINT32*)tmp;
		}

		if (!glyphEntry->pixels)
		{
			WLog_ERR(TAG, "glyphEntry->pixels=NULL");
			return FALSE;
		}

		if (ppGlyphData)
			*ppGlyphData = (BYTE*)glyphEntry->pixels;

		return TRUE;
	}

	return TRUE;
}