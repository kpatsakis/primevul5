static BOOL update_gdi_cache_glyph(rdpContext* context, const CACHE_GLYPH_ORDER* cacheGlyph)
{
	UINT32 i;
	rdpCache* cache;

	if (!context || !cacheGlyph || !context->cache)
		return FALSE;

	cache = context->cache;

	for (i = 0; i < cacheGlyph->cGlyphs; i++)
	{
		const GLYPH_DATA* glyph_data = &cacheGlyph->glyphData[i];
		rdpGlyph* glyph;

		if (!glyph_data)
			return FALSE;

		if (!(glyph = Glyph_Alloc(context, glyph_data->x, glyph_data->y, glyph_data->cx,
		                          glyph_data->cy, glyph_data->cb, glyph_data->aj)))
			return FALSE;

		if (!glyph_cache_put(cache->glyph, cacheGlyph->cacheId, glyph_data->cacheIndex, glyph))
		{
			glyph->Free(context, glyph);
			return FALSE;
		}
	}

	return TRUE;
}