static BOOL update_process_glyph(rdpContext* context, const BYTE* data, UINT32 cacheIndex, INT32* x,
                                 INT32* y, UINT32 cacheId, UINT32 flAccel, BOOL fOpRedundant,
                                 const RDP_RECT* bound)
{
	INT32 sx = 0, sy = 0;
	INT32 dx, dy;
	rdpGlyph* glyph;
	rdpGlyphCache* glyph_cache;

	if (!context || !data || !x || !y || !context->graphics || !context->cache ||
	    !context->cache->glyph)
		return FALSE;

	glyph_cache = context->cache->glyph;
	glyph = glyph_cache_get(glyph_cache, cacheId, cacheIndex);

	if (!glyph)
		return FALSE;

	dx = glyph->x + *x;
	dy = glyph->y + *y;

	if (dx < bound->x)
	{
		sx = bound->x - dx;
		dx = bound->x;
	}

	if (dy < bound->y)
	{
		sy = bound->y - dy;
		dy = bound->y;
	}

	if ((dx <= (bound->x + bound->width)) && (dy <= (bound->y + bound->height)))
	{
		INT32 dw = glyph->cx - sx;
		INT32 dh = glyph->cy - sy;

		if ((dw + dx) > (bound->x + bound->width))
			dw = (bound->x + bound->width) - (dw + dx);

		if ((dh + dy) > (bound->y + bound->height))
			dh = (bound->y + bound->height) - (dh + dy);

		if ((dh > 0) && (dw > 0))
		{
			if (!glyph->Draw(context, glyph, dx, dy, dw, dh, sx, sy, fOpRedundant))
				return FALSE;
		}
	}

	if (flAccel & SO_CHAR_INC_EQUAL_BM_BASE)
		*x += glyph->cx;

	return TRUE;
}