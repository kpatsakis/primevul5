static BOOL update_process_glyph_fragments(rdpContext* context, const BYTE* data, UINT32 length,
                                           UINT32 cacheId, UINT32 ulCharInc, UINT32 flAccel,
                                           UINT32 bgcolor, UINT32 fgcolor, INT32 x, INT32 y,
                                           INT32 bkX, INT32 bkY, INT32 bkWidth, INT32 bkHeight,
                                           INT32 opX, INT32 opY, INT32 opWidth, INT32 opHeight,
                                           BOOL fOpRedundant)
{
	UINT32 n;
	UINT32 id;
	UINT32 size;
	UINT32 index = 0;
	BYTE* fragments;
	rdpGraphics* graphics;
	rdpGlyphCache* glyph_cache;
	rdpGlyph* glyph;
	RDP_RECT bound;

	if (!context || !data || !context->graphics || !context->cache || !context->cache->glyph)
		return FALSE;

	graphics = context->graphics;
	glyph_cache = context->cache->glyph;
	glyph = graphics->Glyph_Prototype;

	if (!glyph)
		return FALSE;

	/* Limit op rectangle to visible screen. */
	if (opX < 0)
	{
		opWidth += opX;
		opX = 0;
	}

	if (opY < 0)
	{
		opHeight += opY;
		opY = 0;
	}

	if (opWidth < 0)
		opWidth = 0;

	if (opHeight < 0)
		opHeight = 0;

	/* Limit bk rectangle to visible screen. */
	if (bkX < 0)
	{
		bkWidth += bkX;
		bkX = 0;
	}

	if (bkY < 0)
	{
		bkHeight += bkY;
		bkY = 0;
	}

	if (bkWidth < 0)
		bkWidth = 0;

	if (bkHeight < 0)
		bkHeight = 0;

	if (opX + opWidth > (INT64)context->settings->DesktopWidth)
	{
		/**
		 * Some Microsoft servers send erroneous high values close to the
		 * sint16 maximum in the OpRight field of the GlyphIndex, FastIndex and
		 * FastGlyph drawing orders, probably a result of applications trying to
		 * clear the text line to the very right end.
		 * One example where this can be seen is typing in notepad.exe within
		 * a RDP session to Windows XP Professional SP3.
		 * This workaround prevents resulting problems in the UI callbacks.
		 */
		opWidth = context->settings->DesktopWidth - opX;
	}

	if (bkX + bkWidth > (INT64)context->settings->DesktopWidth)
	{
		/**
		 * Some Microsoft servers send erroneous high values close to the
		 * sint16 maximum in the OpRight field of the GlyphIndex, FastIndex and
		 * FastGlyph drawing orders, probably a result of applications trying to
		 * clear the text line to the very right end.
		 * One example where this can be seen is typing in notepad.exe within
		 * a RDP session to Windows XP Professional SP3.
		 * This workaround prevents resulting problems in the UI callbacks.
		 */
		bkWidth = context->settings->DesktopWidth - bkX;
	}

	bound.x = bkX;
	bound.y = bkY;
	bound.width = bkWidth;
	bound.height = bkHeight;

	if (!glyph->BeginDraw(context, opX, opY, opWidth, opHeight, bgcolor, fgcolor, fOpRedundant))
		return FALSE;

	if (!IFCALLRESULT(TRUE, glyph->SetBounds, context, bkX, bkY, bkWidth, bkHeight))
		return FALSE;

	while (index < length)
	{
		const UINT32 op = data[index++];

		switch (op)
		{
			case GLYPH_FRAGMENT_USE:
				if (index + 1 >= length)
					return FALSE;

				id = data[index++];
				fragments = (BYTE*)glyph_cache_fragment_get(glyph_cache, id, &size);

				if (fragments == NULL)
					return FALSE;

				for (n = 0; n < size;)
				{
					const UINT32 fop = fragments[n++];
					n = update_glyph_offset(fragments, size, n, &x, &y, ulCharInc, flAccel);

					if (!update_process_glyph(context, fragments, fop, &x, &y, cacheId, flAccel,
					                          fOpRedundant, &bound))
						return FALSE;
				}

				break;

			case GLYPH_FRAGMENT_ADD:
				if (index + 2 > length)
					return FALSE;

				id = data[index++];
				size = data[index++];
				glyph_cache_fragment_put(glyph_cache, id, size, data);
				break;

			default:
				index = update_glyph_offset(data, length, index, &x, &y, ulCharInc, flAccel);

				if (!update_process_glyph(context, data, op, &x, &y, cacheId, flAccel, fOpRedundant,
				                          &bound))
					return FALSE;

				break;
		}
	}

	return glyph->EndDraw(context, opX, opY, opWidth, opHeight, bgcolor, fgcolor);
}