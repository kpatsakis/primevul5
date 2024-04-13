static BOOL update_gdi_cache_glyph_v2(rdpContext* context, const CACHE_GLYPH_V2_ORDER* cacheGlyphV2)
{
	UINT32 i;
	rdpCache* cache;

	if (!context || !cacheGlyphV2 || !context->cache)
		return FALSE;

	cache = context->cache;

	for (i = 0; i < cacheGlyphV2->cGlyphs; i++)
	{
		const GLYPH_DATA_V2* glyphData = &cacheGlyphV2->glyphData[i];
		rdpGlyph* glyph;

		if (!glyphData)
			return FALSE;

		glyph = Glyph_Alloc(context, glyphData->x, glyphData->y, glyphData->cx, glyphData->cy,
		                    glyphData->cb, glyphData->aj);

		if (!glyph)
			return FALSE;

		if (!glyph_cache_put(cache->glyph, cacheGlyphV2->cacheId, glyphData->cacheIndex, glyph))
		{
			glyph->Free(context, glyph);
			return FALSE;
		}
	}

	return TRUE;
}