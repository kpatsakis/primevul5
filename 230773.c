rdpGlyphCache* glyph_cache_new(rdpSettings* settings)
{
	int i;
	rdpGlyphCache* glyphCache;
	glyphCache = (rdpGlyphCache*)calloc(1, sizeof(rdpGlyphCache));

	if (!glyphCache)
		return NULL;

	glyphCache->log = WLog_Get("com.freerdp.cache.glyph");
	glyphCache->settings = settings;
	glyphCache->context = ((freerdp*)settings->instance)->update->context;

	for (i = 0; i < 10; i++)
	{
		glyphCache->glyphCache[i].number = settings->GlyphCache[i].cacheEntries;
		glyphCache->glyphCache[i].maxCellSize = settings->GlyphCache[i].cacheMaximumCellSize;
		glyphCache->glyphCache[i].entries =
		    (rdpGlyph**)calloc(glyphCache->glyphCache[i].number, sizeof(rdpGlyph*));

		if (!glyphCache->glyphCache[i].entries)
			goto fail;
	}

	glyphCache->fragCache.entries = calloc(256, sizeof(FRAGMENT_CACHE_ENTRY));

	if (!glyphCache->fragCache.entries)
		goto fail;

	return glyphCache;
fail:
	glyph_cache_free(glyphCache);
	return NULL;
}