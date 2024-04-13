static BOOL update_gdi_fast_glyph(rdpContext* context, const FAST_GLYPH_ORDER* fastGlyph)
{
	INT32 x, y;
	BYTE text_data[4] = { 0 };
	INT32 opLeft, opTop;
	INT32 opRight, opBottom;
	INT32 opWidth = 0, opHeight = 0;
	INT32 bkWidth = 0, bkHeight = 0;
	rdpCache* cache;

	if (!context || !fastGlyph || !context->cache)
		return FALSE;

	cache = context->cache;
	opLeft = fastGlyph->opLeft;
	opTop = fastGlyph->opTop;
	opRight = fastGlyph->opRight;
	opBottom = fastGlyph->opBottom;
	x = fastGlyph->x;
	y = fastGlyph->y;

	if (opBottom == -32768)
	{
		BYTE flags = (BYTE)(opTop & 0x0F);

		if (flags & 0x01)
			opBottom = fastGlyph->bkBottom;

		if (flags & 0x02)
			opRight = fastGlyph->bkRight;

		if (flags & 0x04)
			opTop = fastGlyph->bkTop;

		if (flags & 0x08)
			opLeft = fastGlyph->bkLeft;
	}

	if (opLeft == 0)
		opLeft = fastGlyph->bkLeft;

	if (opRight == 0)
		opRight = fastGlyph->bkRight;

	/* See update_gdi_fast_index opRight comment. */
	if (opRight > (INT64)context->instance->settings->DesktopWidth)
		opRight = (int)context->instance->settings->DesktopWidth;

	if (x == -32768)
		x = fastGlyph->bkLeft;

	if (y == -32768)
		y = fastGlyph->bkTop;

	if ((fastGlyph->cbData > 1) && (fastGlyph->glyphData.aj))
	{
		/* got option font that needs to go into cache */
		rdpGlyph* glyph;
		const GLYPH_DATA_V2* glyphData = &fastGlyph->glyphData;

		glyph = Glyph_Alloc(context, glyphData->x, glyphData->y, glyphData->cx, glyphData->cy,
		                    glyphData->cb, glyphData->aj);

		if (!glyph)
			return FALSE;

		if (!glyph_cache_put(cache->glyph, fastGlyph->cacheId, fastGlyph->data[0], glyph))
		{
			glyph->Free(context, glyph);
			return FALSE;
		}
	}

	text_data[0] = fastGlyph->data[0];
	text_data[1] = 0;

	if (fastGlyph->bkRight > fastGlyph->bkLeft)
		bkWidth = fastGlyph->bkRight - fastGlyph->bkLeft + 1;

	if (fastGlyph->bkBottom > fastGlyph->bkTop)
		bkHeight = fastGlyph->bkBottom - fastGlyph->bkTop + 1;

	if (opRight > opLeft)
		opWidth = opRight - opLeft + 1;

	if (opBottom > opTop)
		opHeight = opBottom - opTop + 1;

	return update_process_glyph_fragments(
	    context, text_data, sizeof(text_data), fastGlyph->cacheId, fastGlyph->ulCharInc,
	    fastGlyph->flAccel, fastGlyph->backColor, fastGlyph->foreColor, x, y, fastGlyph->bkLeft,
	    fastGlyph->bkTop, bkWidth, bkHeight, opLeft, opTop, opWidth, opHeight, FALSE);
}